/*
MIT License
Copyright (c) 2017 Arlen Keshabyan (arlen.albert@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <vector>
#include "live_property.hpp"
#include "json.hpp"

int main()
{
    using namespace std::literals;

    using live_int = nstd::live_property<int>;
    using live_string = nstd::live_property<std::string>;

	live_int int_prop{ "integer property for tests"s }, dummy_int_prop{ "dummy"s };
	std::vector<nstd::signal_slot::connection> conections;
    auto changing_callback = [](auto &&ctx)
    {
        std::cout << "The property '" << ctx.property.name() << "' changing: from [" << ctx.property.value() << "] to [" << ctx.new_value << "]" << std::endl;

        using value_type = typename std::decay<decltype(ctx.property.value())>::type;

        if constexpr (std::is_arithmetic<value_type>::value)
        {
            if (ctx.cancel = ctx.new_value < 0; ctx.cancel)
            {
                std::cout << "<<<negative numbers are not allowed! The change was cancelled by a slot!>>>" << std::endl;
            }
        }
        else if constexpr (std::is_same<value_type, std::string>::value)
        {
            if (ctx.cancel = std::empty(ctx.new_value); ctx.cancel)
            {
                std::cout << "<<<empty strings are not allowed! The change was cancelled by a slot!>>>" << std::endl;
            }
        }
    };
	auto changed_callback = [](auto &&property)
    {
        std::cout << "The property '" << property.name() << "' changed to: " << property.value() << std::endl;
    };

	conections.emplace_back(int_prop.signal_value_changing.connect(changing_callback));
	conections.emplace_back(int_prop.signal_value_changed.connect(changed_callback));

	for (auto &&c : conections) std::cout << "connection name: '" << c.signal().name() << "'" << std::endl;

	int raw_int = 50;
	dummy_int_prop = 222;
	int_prop = 1;
	int_prop = 150;

	std::cout << "...temporary disabling value_changing signal..." << std::endl;
	conections[0].signal().enabled(false);

	int_prop = raw_int;
	int_prop *= 7;

	std::cout << "...checking for oprator== works in C++17 as expected..." << std::endl;
	std::cout << "comparing int_prop == dummy_int_prop (expecting: false): " << std::boolalpha << (int_prop == dummy_int_prop) << std::endl;

	std::cout << "...enabling value_changing signal again..." << std::endl;
	conections[0].signal().enabled(true);

	int_prop = dummy_int_prop;

	std::cout << "now comparing int_prop == dummy_int_prop (expecting: true): " << (int_prop == dummy_int_prop) << std::endl;

	int_prop = -1;
	std::cout << "int_prop = " << int_prop << std::endl;

	std::cout << "testing ++ and --: " << std::endl;
    ++int_prop;
    int_prop++;
    --int_prop;
    int_prop--;

	conections.clear(); //auto-disconnect from all slots
    std::cout << "no slots are called from now on since we destroied all connections..." << std::endl << "...setting int_prop to -1 should not be restricted now..." << std::endl;

    int_prop = -1;

    std::cout << "int_prop = " << int_prop << std::endl;

    live_string str_prop{ "string property for tests"s, "___"s }, dummy_string_prop{ "dummy"s };

	conections.emplace_back(str_prop.signal_value_changing.connect(changing_callback));
	conections.emplace_back(str_prop.signal_value_changed.connect(changed_callback));

	str_prop = "Hello World!"s;
	str_prop = ""s;

	std::cout << "str_prop = " << str_prop.value() << std::endl;

    nstd::signal_slot::connection ts; //should be out of a signal's scope to be destroyed after it's signal thus letting a signal to emit the rest of queued signals...
    {
        nstd::signal_slot::throttled_signal<std::string> sg("THROTTLED"s, 50ms);
        ts = sg.connect([&sg](auto &&str){ std::cout << "throttle: " << str << "; " << sg.name() << std::endl; });

        constexpr int sg_count {10};
        for (auto idx{0}; idx < sg_count; ++idx)
            sg.emit("throttled signal emitted..."s);

        std::this_thread::sleep_for(300ms);

        for (auto idx{0}; idx < sg_count; ++idx)
            sg.emit("throttled signal emitted..."s);

        std::cout << "done..." << std::endl;
        std::cout << "emitting the rest of queued signals..." << std::endl;
    }

	std::this_thread::sleep_for(1s);

	nstd::signal_slot::timer_signal<live_string*> timer("My timer"s, 500ms);

	int idx { 0 };
	conections.emplace_back(timer.connect([&idx](auto &&s, auto &&p)
    {
        std::cout << "timer: "s << s->name() << std::endl;
        *p = std::to_string(++idx) + " tick..."s;

        if (idx == 2)
        {
            s->timer_ms(200ms);
            *p = "...timer duration changed to 200ms"s;
        }

        if (idx >= 5)
        {
            s->disable_timer_from_slot();
            *p = "...timer stoped... sleeping for some time..."s;
        }
    }));
	timer.start_timer(&str_prop);

	std::this_thread::sleep_for(5s);

	nstd::signal_slot::signal<std::string> jsig("JSON signal"s);

	auto jcon = jsig.connect([](auto &&jstr)
    {
        auto j = nstd::json::json::parse(jstr);

        //std::cout << "JSON property: " << j["/JSONObject/property"_json_pointer] <<std::endl;
        std::cout << "JSON property: " << j["JSONObject"]["property"] <<std::endl;
    });

    nstd::json::json params, rj = {{"JSONObject"s, {{"property"s, "This is the super JSON property..."s}, {"One_more_property"s, 888}, {"Niels Lohmann does amazing json for cpp", true}}}};
    params["JSONObject"s] = {{"property"s, "This is the real JSON property..."s}};

    jsig.emit(params.dump());
    jsig.emit((R"({"JSONObject": {"property": "This is the parsed JSON property..."}})"_json).dump());
    jsig.emit(rj.dump());

    std::cout << "Pretty printed JSON:" << std::endl << std::setw(3) << rj << std::endl;

    nstd::signal_slot::signal_set<std::string> ss;
    auto z = ss.connect("/mainwindow/button/ok"s, [](auto &&s){ std::cout << s << std::endl; });
    auto zz = ss["/new/channel"s]->connect([](auto &&s){ std::cout << s << std::endl; });
    for (auto &&sn : ss.get_signal_names()) std::cout << "signal name: " << sn << std::endl;
    ss.emit("hello..."s);

	std::cout << "exitting..." << std::endl;

    return 0;
}

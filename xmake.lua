target("extmem")
	set_languages("c++17")
	set_kind("static")
	add_includedirs("./")
	add_files("./extmem.cpp")

target("test-extmem")
	add_deps("extmem")
	add_files("./test.cpp")
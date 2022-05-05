// vim: set noet ts=4 sts=4 sw=4:
#pragma once

#include <vector>
#include <ostream>
#include <iomanip>
#include <cstdio>
#include <chrono>

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

namespace profiler {

class single_profiler;

class profiler {
	std::vector<single_profiler *> profs;
	int indent;
	profiler() { }
	const std::string format_time(double v) const {
		char buf[1024];
		if (v < 1e-6)
			sprintf(buf, "%8.2lfns", 1e9 * v);
		else if (v < 1e-3)
			sprintf(buf, "%8.2lfus", 1e6 * v);
		else if (v < 1)
			sprintf(buf, "%8.2lfms", 1e3 * v);
		else
			sprintf(buf, "%8.2lfs ", v);
		return std::string(buf);
	}
public:
	static profiler &getInstance() {
		static profiler instance;
		return instance;
	}
	void clear();
	void reg(single_profiler *prof) {
		profs.push_back(prof);
	}
	int enter() { return indent++; }
	void leave() { indent--; }
	std::ostream &print(std::ostream &o, int fnwidth) const;
	profiler(const profiler &) = delete;
	profiler(profiler &&) = delete;
	profiler &operator=(const profiler &) = delete;
};

inline const profiler &getInstance() {
	return profiler::getInstance();
}

class single_profiler {
	std::chrono::time_point<std::chrono::high_resolution_clock> startmark;
	int _calls;
	double tottime;
	int depth;
	int indent;
	void start() {
		startmark = std::chrono::high_resolution_clock::now();
	}
	double stop() {
		std::chrono::time_point<std::chrono::high_resolution_clock> stopmark;
		stopmark = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(startmark).time_since_epoch().count();
		auto stop = std::chrono::time_point_cast<std::chrono::nanoseconds>(stopmark).time_since_epoch().count();
		return 1e-9 * (stop - start);
	}
	const std::string _filename;
	const std::string _pretty_function;
	const int _line_number;
public:
	single_profiler(const char *fn, const int line, const char *func)
		: _calls(0), tottime(0), depth(0), _filename(fn)
		, _pretty_function(func), _line_number(line)
	{
		profiler::getInstance().reg(this);
	}
	void enter() {
		_calls++;
		if (depth > 0)
			tottime += stop(); // suspend
		else
			indent = profiler::getInstance().enter();
		depth++;
		start();
	}
	void leave() {
		tottime += stop();
		depth--;
		if (depth > 0)
			start(); // resume
		else
			profiler::getInstance().leave();
	}
	void clear(){
		_calls = 0;
		tottime = 0;
		depth = 0;
	}
	const std::string &filename() const { return _filename; }
	int line_number() const { return _line_number; }
	const std::string function() const { return std::string(indent, ' ') + _pretty_function; }
	int calls() const { return _calls; }
	double total_time() const { return tottime; }
};

inline void profiler::clear(){
	for (auto& p : profs)
		p->clear();
}

template<typename T>
void profile_gate(const char *fn, const int line, const char *func, bool enter, T *) {
	static single_profiler prof(fn, line, func);
	if (enter)
		prof.enter();
	else
		prof.leave();
}

inline std::ostream &profiler::print(std::ostream &o, int fnwidth) const {
	o << "\n" << std::left << std::setw(fnwidth) << "function";
	o << "line	calls	   tottime	avgtime\n\n";
	for (const single_profiler *p : profs) {
		o << std::left << std::setfill('.') << std::setw(fnwidth) << (p->function() + ":" + p->filename()) << std::setfill(' ')
			<< " " << std::setw(8) << p->line_number()
			<< " " << std::setw(8) << p->calls() << " "
			<< format_time(p->total_time()) << " " << format_time(p->total_time() / p->calls()) << "\n";
	}
	return o;
}

inline std::ostream &operator<<(std::ostream &o, const profiler &prof) {
	return prof.print(o, 120);
}

}

#define PROFILE_AS(name) \
	struct __prof_data { bool early_exit; \
		__prof_data(const char *fn, const int line, const char *func) { \
			early_exit = false; \
			profiler::profile_gate(fn, line, func, true, this); \
		} \
		void stop() { \
			early_exit = true; \
			profiler::profile_gate(nullptr, -1, nullptr, false, this); \
		} \
		~__prof_data() { \
			if (!early_exit) \
				stop(); \
		} \
	} __helper_var(__FILENAME__, __LINE__, name)
#define PROFILE PROFILE_AS(__FUNCTION__)
#define PROFILE_SIG PROFILE_AS(__PRETTY_FUNCTION__)
#define PROFILE_END \
	__helper_var.stop()


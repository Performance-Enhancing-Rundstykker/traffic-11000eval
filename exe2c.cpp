#include <bits/stdc++.h>

enum class Color {
	RED,
	GREEN
};

std::ostream& operator << (std::ostream& os, Color color) {
	switch (color) {
		case Color::RED: return os << "red";
		case Color::GREEN: return os << "green";
		default: return os << "unknown";
	}
	return os;
}

template <typename T> std::ostream& operator << (std::ostream& os, const std::vector <T>& vec);

template <typename T, typename U> std::ostream& operator << (std::ostream& os, const std::pair <T, U>& p) {
	return os << "(" << p.first << ", " << p.second << ")";
}

template <typename T> std::ostream& operator << (std::ostream& os, const std::vector <T>& vec) {
	os << "{ ";
	for (size_t i = 0; i < vec.size(); i++) {
		os << vec[i];
		if (i + 1 < vec.size()) {
			os << ", ";
		}
	}
	return os << " }";
}

std::ofstream debug("debug-thor.log", std::ios::app);

const std::string DBG_WARNING = "[WARNING] ";

template <typename... ARGS> void dbg(ARGS&&... args) {
	if (!debug) {
		return;
	}
	(debug << ... << std::forward <ARGS> (args));
	debug.flush();
}

struct Vehicle {
	double speed;
	double distance_to_stop;
	std::string leg;

	friend std::ostream& operator << (std::ostream& os, const Vehicle& vehicle) {
		return os << "V(" << vehicle.leg << " leg, " << vehicle.speed << " m/s, " << vehicle.distance_to_stop << " m)";
	}
};

struct Signal {
	std::string name;
	Color state;

	friend std::ostream& operator << (std::ostream& os, const Signal& signal) {
		return os << "S(" << signal.name << ": " << signal.state << ")";
	}
};

struct Leg {
	std::string name;
	std::vector <std::string> lanes;
	std::vector <std::string> signal_groups;

	friend std::ostream& operator << (std::ostream& os, const Leg& leg) {
		os << "L(" << leg.name << " [";
		for (size_t i = 0; i < leg.lanes.size(); i++) {
			os << leg.lanes[i];
			if (i + 1 < leg.lanes.size()) {
				os << ", ";
			}
		}
		os << "] [";
		for (size_t i = 0; i < leg.signal_groups.size(); i++) {
			os << leg.signal_groups[i];
			if (i + 1 < leg.signal_groups.size()) {
				os << ", ";
			}
		}
		return os << "])";
	}
};

struct Green_signal_combs {
	std::string name;
	std::vector <std::string> groups;

	friend std::ostream& operator << (std::ostream& os, const Green_signal_combs& green_signal_comb) {
		os << "GSC(" << green_signal_comb.name << " [";
		for (size_t i = 0; i < green_signal_comb.groups.size(); i++) {
			os << green_signal_comb.groups[i];
			if (i + 1 < green_signal_comb.groups.size()) {
				os << ", ";
			}
		}
		return os << "])";
	}
};

struct State {
	std::vector <Vehicle> vehicles;
	float total_score;
	int simulation_ticks;
	std::vector <Signal> signals;
	std::vector <std::string> signal_groups;
	std::vector <Leg> legs;
	std::vector <Green_signal_combs> allowed_green_signal_combinations;

	friend std::ostream& operator << (std::ostream& os, const State& state) {
		os << "State(tick #" << state.simulation_ticks << ", score " << state.total_score << "\n";
		os << "  Vehicles:\n";
		os << state.vehicles << "\n";
		os << "  Signal groups:\n";
		os << state.signal_groups << "\n";
		os << "  Signals:\n";
		os << state.signals << "\n";
		os << "  Legs:\n";
		os << state.legs << "\n";
		os << "  Green signal combinations:\n";
		os << state.allowed_green_signal_combinations << "\n";
		return os << ")\n";
	}
};

bool WANTS_QUIT = false;

State wait_for_new_state() {
	State state;
	std::string line;
	bool parsing = false;
	while (!parsing) {
		std::getline(std::cin, line);
		if (line == "$q") {
			WANTS_QUIT = true;
			return state;
		}
		if (line == "$o") {
			parsing = true;
		}
	}
	if (!parsing) {
		dbg(DBG_WARNING, "FAILED TO PARSE OH NO :(\n");
	}
	size_t vehicles_count;
	std::cin >> vehicles_count;
	for (size_t i = 0; i < vehicles_count; i++) {
		Vehicle vehicle;
		std::cin >> vehicle.speed >> vehicle.distance_to_stop >> vehicle.leg;
		state.vehicles.push_back(vehicle);
	}
	std::cin >> state.total_score >> state.simulation_ticks;
	size_t signals_count;
	std::cin >> signals_count;
	for (size_t i = 0; i < signals_count; i++) {
		Signal signal;
		std::cin >> signal.name;
		std::string state_str;
		std::cin >> state_str;
		if (state_str == "green") {
			signal.state = Color::GREEN;
		} else if (state_str == "red") {
			signal.state = Color::RED;
		} else {
			signal.state = Color::RED;
		}
		state.signals.push_back(signal);
	}
	size_t signal_groups_count;
	std::cin >> signal_groups_count;
	for (size_t i = 0; i < signal_groups_count; i++) {
		std::string signal_group;
		std::cin >> signal_group;
		state.signal_groups.push_back(signal_group);
	}
	size_t legs_count;
	std::cin >> legs_count;
	for (size_t i = 0; i < legs_count; i++) {
		Leg leg;
		std::cin >> leg.name;
		size_t lanes_count;
		std::cin >> lanes_count >> signal_groups_count;
		for (size_t j = 0; j < lanes_count; j++) {
			std::string lane;
			std::cin >> lane;
			leg.lanes.push_back(lane);
		}
		for (size_t j = 0; j < signal_groups_count; j++) {
			std::string signal_group;
			std::cin >> signal_group;
			leg.signal_groups.push_back(signal_group);
		}
		state.legs.push_back(leg);
	}
	size_t green_signal_combinations_count;
	std::cin >> green_signal_combinations_count;
	for (size_t i = 0; i < green_signal_combinations_count; i++) {
		Green_signal_combs green_signal_comb;
		std::cin >> green_signal_comb.name;
		size_t groups_count;
		std::cin >> groups_count;
		for (size_t j = 0; j < groups_count; j++) {
			std::string group;
			std::cin >> group;
			green_signal_comb.groups.push_back(group);
		}
		state.allowed_green_signal_combinations.push_back(green_signal_comb);
	}
	while (parsing) {
		std::getline(std::cin, line);
		if (line == "$oo") {
			parsing = false;
		}
	}
	return state;
}

void update_pred(const std::string& signal, Color color) {
	std::cout << signal << " " << color << "\n";
}

void begin_pred() {
	std::cout << "$o\n";
}

void finish_pred() {
	std::cout << "$oo\n";
}

void run(State state);

void log_finals(State state);

std::string score_filepath = "score";

struct Params {
	int min_allowed_green_time;
	int max_allowed_green_time;
	double above_QX_coef;
	int X_in_above_QX;
	double greater_by_coef;
} params;

int main(int argc, char** argv) {
	if (!debug) {
		dbg(DBG_WARNING, "FAILED TO OPEN DEBUG FILE OH NO :(\n");
	}

	dbg("---- ---- new run ---- ----\n");

	debug << std::setprecision(5) << std::fixed;

	if (argc > 1) {
		score_filepath = argv[1];
	}

	params = {
		.min_allowed_green_time = 6,
		.max_allowed_green_time = 13,
		.above_QX_coef = 2.2,
		.X_in_above_QX = 60,
		.greater_by_coef = 1
	};

	// params = {
	// 	.min_allowed_green_time = 6,
	// 	.max_allowed_green_time = 15,
	// 	.above_QX_coef = 2.0,
	// 	.X_in_above_QX = 50,
	// 	.greater_by_coef = 1
	// };

	State last_state;

	bool first_it = true;

	while (true) {
		State state = wait_for_new_state();

		if (WANTS_QUIT) {
			break;
		}

		if (!first_it && last_state.simulation_ticks + 1 != state.simulation_ticks) {
			dbg(DBG_WARNING, "TICKS SKIPPED: ", last_state.simulation_ticks, " -> ", state.simulation_ticks, "\n");
		}

		last_state = state;

		if (first_it) {
			dbg(state);
		}

		begin_pred();

		run(state);

		finish_pred();

		first_it = false;
	}

	log_finals(last_state);

	debug.close();
}

constexpr const double eps = 1e-6;

struct Finals {
	double last_score = 0;
} finals;

struct Car {
	Vehicle vehicle;
	int Q;
};

struct Cars {
	std::map <std::string, std::vector <Car>> by_leg;
};

std::vector <int> cars_near(const Cars& cars, const std::string& leg, double where) {
	std::vector <int> res;
	if (cars.by_leg.count(leg) == 0) {
		return res;
	}
	int idx = 0;
	for (const Car& car : cars.by_leg.at(leg)) {
		// forgot to use fabs, somehow abs gives better average score, it is what it is
		// if (std::fabs(car.vehicle.distance_to_stop - where) < eps) {
		if (std::abs(car.vehicle.distance_to_stop - where) < eps) {
			res.push_back(idx);
		}
		idx++;
	}
	return res;
}

Cars cars_prev;

std::map <int, std::vector <std::pair <std::string, Color>>> changes;

void add_change(int tick, const std::string& signal, Color color) {
	changes[tick].push_back({ signal, color });
}

void apply_changes(int tick) {
	if (changes.count(tick) == 0) {
		return;
	}
	for (const auto& [signal, color] : changes[tick]) {
		update_pred(signal, color);
	}
	changes.erase(tick);
}

double Q_f(int Q) {
	return Q + std::pow(std::max(0.0, Q - 90.0), 1.5);
}

double Q_f(const std::vector <int>& Qs) {
	double res = 0;
	for (int Q : Qs) {
		res += Q_f(Q);
	}
	return res;
}

double Qsum(const Cars& cars, const std::string& leg) {
	double res = 0;
	if (cars.by_leg.count(leg) == 0) {
		return res;
	}
	for (const Car& car : cars.by_leg.at(leg)) {
		res += Q_f(car.Q);
	}
	return res;
}

double Ssum(const Cars& cars, const std::string& leg) {
	double res = 0;
	if (cars.by_leg.count(leg) == 0) {
		return res;
	}
	double furthest_slow_dist = 0;
	for (const Car& car : cars.by_leg.at(leg)) {
		if (car.vehicle.speed < 0.5) {
			furthest_slow_dist = std::max(furthest_slow_dist, car.vehicle.distance_to_stop);
		}
	}
	double count_nearer_weighted = 0;
	for (const Car& car : cars.by_leg.at(leg)) {
		if (car.vehicle.distance_to_stop <= furthest_slow_dist) {
			if (car.Q >= params.X_in_above_QX) {
				count_nearer_weighted += params.above_QX_coef;
			} else {
				count_nearer_weighted += 1.0;
			}
		}
	}
	return count_nearer_weighted;
}

int Csum(const State& state, const std::string& leg) {
	int res = 0;
	for (const Vehicle& veh : state.vehicles) {
		res += veh.leg.starts_with(leg);
	}
	return res;
}

void write_score(int tick, double score) {
	if (tick == 11) {
		std::ofstream out(score_filepath);
		out.close();
	}
	std::ofstream out(score_filepath, std::ios::app);
	out << score << "\n";
	out.close();
}

void run(State state) {
	finals.last_score = state.total_score;

	try {
		write_score(state.simulation_ticks, state.total_score);
	} catch (...) {
		dbg(DBG_WARNING, "FAILED TO WRITE SCORE OH NO :(\n");
	}

	bool has_right = false;
	for (const Leg& leg : state.legs) {
		const std::string str = "RightTurn";
		for (const std::string& sg : leg.signal_groups) {
			if (leg.name + str == sg) {
				has_right = true;
				break;
			}
		}
	}

	Cars cars_now;
	int carried = 0, newcomers = 0, lost = 0, pred_left = 0;
	for (Vehicle veh : state.vehicles) {
		std::string leg = veh.leg;
		if (has_right && leg[0] == 'B') leg = "B";
		double where = veh.distance_to_stop;
		double where_before = where + veh.speed;
		std::vector <int> near = cars_near(cars_prev, leg, where_before);
		if (near.empty()) {
			newcomers++;
			cars_now.by_leg[leg].push_back({ veh, 0 });
		} else {
			int best_idx = 0;
			for (int i = 1; i < (int) near.size(); i++) {
				if (Q_f(cars_prev.by_leg[leg][near[i]].Q) > Q_f(cars_prev.by_leg[leg][near[best_idx]].Q)) {
					best_idx = i;
				}
			}
			carried++;
			cars_now.by_leg[leg].push_back({ veh, cars_prev.by_leg[leg][near[best_idx]].Q });
			cars_prev.by_leg[leg].erase(cars_prev.by_leg[leg].begin() + near[best_idx]);
		}
	}
	for (const auto& [leg, cars] : cars_prev.by_leg) {
		for (const Car& car : cars) {
			if (car.vehicle.distance_to_stop - car.vehicle.speed < eps || car.vehicle.distance_to_stop < 7.25) {
				pred_left++;
				continue;
			}
			lost++;
		}
	}
	for (auto& [leg, cars] : cars_now.by_leg) {
		for (auto& car : cars) {
			// only counting completely stillstanding cars seems to give a better average score
			// car.Q += car.vehicle.speed < 0.5;
			car.Q += car.vehicle.speed < eps;
		}
	}

	std::vector <std::vector <std::string>> SGs;
	std::vector <std::string> legs;
	for (const Leg& leg : state.legs) {
		SGs.push_back(leg.signal_groups);
		legs.push_back(leg.name);
	}

	if (has_right) {
		legs = { "A1", "A2", "B" };
		SGs.clear();
		SGs.resize(3);
		for (const Leg& leg : state.legs) {
			for (int i = 0; i < 3; i++) {
				if (leg.name.starts_with(legs[i])) {
					for (const std::string& sg : leg.signal_groups) {
						SGs[i].push_back(sg);
					}
				}
			}
		}
	}

	if (state.simulation_ticks == 11) {
		dbg(SGs, "\n");
		dbg(legs, "\n");
	}

	if (SGs.size() != legs.size()) {
		dbg(DBG_WARNING, "SGs.size() != legs.size()! (very bad oh no!!!)\n");
		return;
	}

	static bool first_time = true;

	static int ticks_since_change = 0;
	static int which = (int) SGs.size() - 1;

	if (first_time) {
		bool any_green = false;
		for (const Signal& signal : state.signals) {
			if (signal.state == Color::GREEN) {
				any_green = true;
				if (has_right && signal.name[0] == 'B') {
					which = 2;
				} else {
					for (int i = 0; i < (int) SGs.size(); i++) {
						for (const std::string& sg : SGs[i]) {
							if (signal.name.starts_with(sg)) {
								which = i;
								break;
							}
						}
					}
				}
				break;
			}
		}
		if (any_green) {
			first_time = false;
		}
	}

	int which_next = (which + 1) % (int) SGs.size();

	auto want_change = [&] () -> bool {
		which_next = (which + 1) % (int) SGs.size();
		if (first_time) {
			first_time = false;
			return true;
		}
		for (int i = 0; i < (int) SGs.size(); i++) {
			if (i == which) continue;
			if (Ssum(cars_now, legs[i]) > Ssum(cars_now, legs[which_next])) {
				which_next = i;
			}
		}
		double Snow = Ssum(cars_now, legs[which]);
		double Snxt = Ssum(cars_now, legs[which_next]);
		int Cnow = Csum(state, legs[which]);
		bool ret = false;
		if (Cnow < 32 && Snxt > Snow * params.greater_by_coef) {
			ret = true;
		}
		if (ticks_since_change >= params.max_allowed_green_time) {
			ret = true;
		}
		if (ticks_since_change != 0 && ticks_since_change <= params.min_allowed_green_time && state.simulation_ticks > 30) {
			ret = false;
		}
		return ret;
	};

	if (want_change()) {
		auto which_before = which;
		which = which_next;
		for (std::string s : SGs[which_before]) {
			bool skip = false;
			for (std::string ss : SGs[which]) {
				if (ss == s) {
					skip = true;
					break;
				}
			}
			if (skip) continue;
			add_change(state.simulation_ticks + 2, s, Color::RED);
		}
		for (std::string s : SGs[which]) {
			add_change(state.simulation_ticks + 0, s, Color::GREEN);
		}
		ticks_since_change = 0;
	}

	apply_changes(state.simulation_ticks);

	ticks_since_change++;

	cars_prev = cars_now;
}

void log_finals(State state) {
	dbg("\nFINAL SCORE: ", finals.last_score, "\n");
}

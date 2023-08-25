#include <cmath>
#include <string>
#include <vector>
#include <deque>

// Code modified from https://github.com/RyanMarcus/plr

struct point {
    double x;
    double y;

    point() = default;
    point(double x, double y) : x(x), y(y) {}
};

struct line {
    double a;
    double b;

    bool LineCheck() {
        if (!std::isnan(a) && !std::isnan(b)) {
            return true;
        }
        return false;
    }
};

class Segment {
public:
    Segment(uint64_t _x, double _k, double _b) : x(_x), k(_k), b(_b) {}
    uint64_t x;
    double k;
    double b;
};

double get_slope(struct point p1, struct point p2);
struct line get_line(struct point p1, struct point p2);
struct point get_intersetction(struct line l1, struct line l2);

bool is_above(struct point pt, struct line l);
bool is_below(struct point pt, struct line l);

struct point get_upper_bound(struct point pt, double gamma);
struct point get_lower_bound(struct point pt, double gamma);


class GreedyPLR {
private:
    std::string state;
    double gamma;
    struct point last_pt;
    struct point s0;
    struct point s1;
    struct line rho_lower;
    struct line rho_upper;
    struct point sint;

    bool setup();
    Segment current_segment();
    Segment process__(struct point pt, bool file);

public:
    GreedyPLR(double gamma);
    Segment process(const struct point& pt, bool file);
    Segment finish();
};

class PLR {
private:
    double gamma;
    std::vector<Segment> segments;

public:
    PLR(double gamma);
    std::vector<Segment>& train(std::vector<std::string>& keys, bool file);
//    std::vector<double> predict(std::vector<double> xx);
//    double mae(std::vector<double> y_true, std::vector<double> y_pred);
};

#include "sweepline.hpp"
#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

struct IntersectionCallback {
    size_t count;

    IntersectionCallback(): count(0){}

    void operator () (
        const Point &,
        const std::vector<const Segment*> &
    ){
        count++;
    }
};

int main(){
    for (size_t n = 500; n <= 10000; n += 500){
        std::vector<Segment> segments;

        for (size_t i = 0; i < n; i++){
            int ax = 0;
            int ay = i;
            int bx = 10000;
            int by = ay + 100;

            segments.push_back(Segment(Point(ax, ay), Point(bx, by)));
        }

        segments.push_back(Segment(Point(10000, 0), Point(0, 10000)));
        //segments.push_back(Segment(Point(0, 0), Point(10000, 10000)));

        double start_time = sec();

        IntersectionCallback callback;

        find_intersections_sweepline(segments, callback);

        double elapsed_time = sec() - start_time;

        std::cout << n << " segments " << callback.count << " intersections " << elapsed_time << " seconds" << std::endl;
    }

    return 0;
}


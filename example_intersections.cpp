#include "sweepline.hpp"

int main(){
    std::vector<Segment> segments = {
        Segment(Point(0, 0), Point(1, 1)),
        Segment(Point(1, 0), Point(0, 1)),
    };

    std::vector<Point> intersections = find_intersections_sweepline(segments);

    for (const Point &intersection : intersections){
        std::cout << intersection << std::endl;
    }

    return 0;
}

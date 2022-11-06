#include "sweepline.hpp"

struct IntersectionCallback {
    void operator () (
        const Point &intersection,
        const std::vector<const Segment*> &segments
    ){
        std::cout << "Intersection " << intersection.x << " " << intersection.y << std::endl;
        for (const Segment *segment : segments){
            std::cout << "Segment " << segment->a.x << " " << segment->a.y;
            std::cout << " " << segment->b.x << " " << segment->b.y << std::endl;
        }
        std::cout << std::endl;
    }
};

int main(){
    std::cout <<
        "Enter segments as 4 numbers. For example, the segment "
        "((1, 2), (3, 4)) should be entered as 1 2 3 4. Press "
        "Ctrl + D to end your input. The output will contain "
        "intersection points and corresponding segments." << std::endl;

    std::vector<Segment> segments;

    Point a, b;
    while (std::cin >> a.x >> a.y >> b.x >> b.y){
        segments.emplace_back(Segment(a, b));
    }

    std::cout << std::endl;

    IntersectionCallback callback;

    find_intersections_sweepline(segments, callback);

    return 0;
}

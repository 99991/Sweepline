#include "sweepline.hpp"

struct IntersectionCallback {
    void operator () (
        const Point &intersection,
        const std::vector<const Segment*> &segments
    ){
        std::cout << "New intersection at " << intersection << " between " << segments.size() << " line segments:" << std::endl;
        for (const Segment *segment : segments){
            std::cout << "Segment " << segment->a << ", " << segment->b << std::endl;
        }
        std::cout << std::endl;
    }
};

int main(){
    std::vector<Segment> segments = {
        Segment(Point(0, 0), Point(1, 1)),
        Segment(Point(1, 0), Point(0, 1)),
    };

    IntersectionCallback callback;

    find_intersections_sweepline(segments, callback);

    return 0;
}

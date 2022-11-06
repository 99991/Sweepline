# Sweepline

This code computes intersections between line segments in `O((n + k) log n)` where `n` is the number of segments and `k` is the number of intersections.

The code is not production-ready!
It handles all degenerate cases I could come up with and asymptotic performance seems reasonable,
but the implicit constant is much too large for most practical applications.

# Compilation

The following command will compile `main`, `benchmark`, `example_segments` and `example_intersections`.

```bash
make
```

# Usage example

Also see `example_intersections.cpp` and `example_segments.cpp`.

If you only want the points of intersection, you can use the `find_intersections_sweepline` function.

```c++
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
```

If you also want all segments participating in an intersection, you can use the callback-based interface.

```c++
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
```

# Run tests

```bash
python3 test_sweepline.py
```

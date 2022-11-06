#include "intrusive_list.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <gmpxx.h>

typedef mpq_class Fraction;

struct Point {
    Fraction x, y;

    Point(){}
    Point(const Fraction &x, const Fraction &y): x(x), y(y){}
};

typedef std::vector<Point> Points;

std::ostream& operator << (std::ostream &out, Point p) {
    out << "(" << p.x << ", " << p.y << ")";
    return out;
}

Point operator + (Point a, Point b){
    return Point{a.x + b.x, a.y + b.y};
}

Point operator - (Point a, Point b){
    return Point{a.x - b.x, a.y - b.y};
}

Point operator * (Fraction a, Point b){
    return Point{a * b.x, a * b.y};
}

bool operator == (Point a, Point b){
    return a.x == b.x && a.y == b.y;
}

bool operator != (Point a, Point b){
    return a.x != b.x || a.y != b.y;
}

Fraction dot(Point a, Point b){
    return a.x * b.x + a.y * b.y;
}

Fraction det(Point a, Point b){
    return a.x * b.y - a.y * b.x;
}

bool operator < (Point a, Point b){
    if (a.x < b.x) return true;
    if (a.x > b.x) return false;
    if (a.y < b.y) return true;
    if (a.y > b.y) return false;
    return false;
}

bool operator > (Point a, Point b){
    if (a.x > b.x) return true;
    if (a.x < b.x) return false;
    if (a.y > b.y) return true;
    if (a.y < b.y) return false;
    return false;
}

bool operator <= (Point a, Point b){
    return !(a > b);
}

bool operator >= (Point a, Point b){
    return !(a < b);
}

bool between(Fraction a, Fraction x, Fraction b){
    return a <= x && x <= b;
}

struct Segment {
    Point a, b;
    IntrusiveNode<Segment> node;
    IntrusiveNode<Segment> end_node;

    Segment(const Segment &seg): a(seg.a), b(seg.b){
        assert(seg.node.unlinked());
    }

    Segment(const Point &a, const Point &b):
        a(a), b(b){}

    Fraction slope() const {
        return (b.y - a.y) / (b.x - a.x);
    }

    bool is_vertical() const {
        return a.x == b.x;
    }

    bool is_point() const {
        return a == b;
    }
};

typedef std::vector<Segment> Segments;

std::ostream& operator << (std::ostream &out, const Segment &s) {
    out << s.a << ", " << s.b;
    return out;
}

bool operator == (const Segment &seg0, const Segment &seg1){
    return seg0.a == seg1.a && seg0.b == seg1.b;
}

template <typename Iterator, typename Value>
bool contains(Iterator a, Iterator b, const Value &value){
    for (; a != b; ++a){
        if (*a == value) return true;
    }
    return false;
}

void find_intersections_two_segments(Point a, Point b, Point c, Point d, Points &intersections){
    Point ba = b - a;
    Point dc = d - c;
    Point ca = c - a;

    Fraction ba_det_dc = det(ba, dc);
    Fraction ca_det_dc = det(ca, dc);
    Fraction ca_det_ba = det(ca, ba);

    // If segments are parallel
    if (ba_det_dc == 0){
        // If parallel segments are on same line
        if (ca_det_ba == 0 && ca_det_dc == 0){
            Fraction ba2 = dot(ba, ba);
            Fraction dc2 = dot(dc, dc);

            // If a == b && c == d
            if (ba2 == 0 && dc2 == 0){
                if (a == c){
                    intersections.push_back(a);
                    intersections.push_back(a);
                }
            }
            // If a == b
            else if (ba2 == 0){
                // Return `a` if `a` lies on `(c, d)`
                if (between(0, dot(dc, a - c), dc2)){
                    intersections.push_back(a);
                }
            }
            // If c == d
            else if (dc2 == 0){
                // Return `c` if `c` lies on `(a, b)`
                if (between(0, dot(ba, c - a), ba2)){
                    intersections.push_back(c);
                }
            }
            else {
                assert(a <= b);
                assert(c <= d);

                Point points[4];
                int n = 0;

                if (between(0, dot(dc, a - c), dc2)) points[n++] = a;
                if (between(0, dot(dc, b - c), dc2)) points[n++] = b;
                if (between(0, dot(ba, c - a), ba2)) points[n++] = c;
                if (between(0, dot(ba, d - a), ba2)) points[n++] = d;

                if (n > 0){
                    Point p = *std::min_element(points, points + n);
                    Point q = *std::max_element(points, points + n);

                    intersections.push_back(p);
                    intersections.push_back(q);
                }
            }
        }
    }else{
        Fraction t = ca_det_ba / ba_det_dc;
        Fraction s = ca_det_dc / ba_det_dc;

        if (0 <= t && t <= 1 && 0 <= s && s <= 1){
            Point intersection = a + s * ba;

            intersections.push_back(intersection);
        }
    }
}

bool operator == (const Points &points1, const Points &points2){
    if (points1.size() != points2.size()) return false;
    for (size_t i = 0; i < points1.size(); i++){
        if (points1[i] != points2[i]) return false;
    }
    return true;
}

bool operator != (const Points &points1, const Points &points2){
    return !(points1 == points2);
}

struct SweepKey {
    const Point &event_point;
    bool after_event_point = false;
    Fraction max_slope;

    SweepKey(const Point &event_point, Fraction max_slope):
        event_point(event_point), max_slope(max_slope){}

    Point operator () (const Segment &seg) const {
        Fraction ey = event_point.y;

        Point key;

        if (seg.is_vertical()){
            Fraction y = std::max(ey, std::min(seg.a.y, seg.b.y));
            y = std::min(y, std::max(seg.a.y, seg.b.y));

            key = Point{y, seg.a.y < seg.b.y ? -max_slope : max_slope};
        }else{
            Fraction slope = seg.slope();
            Fraction intercept = seg.a.y - slope * seg.a.x;
            Fraction y = slope * event_point.x + intercept;

            key = Point{y, (y < ey) ? slope : -slope};
        }

        // The order of segments intersecting the event_point reverses after the event_point
        if (after_event_point){
            key.y = -key.y;
        }

        return key;
    }
};

struct SegmentComparator {
    const SweepKey &get_sweep_key;

    SegmentComparator(const SweepKey &get_sweep_key): get_sweep_key(get_sweep_key){}

    bool operator () (const Segment &seg1, const Segment &seg2) const {
        return get_sweep_key(seg1) < get_sweep_key(seg2);
    }
};

typedef IntrusiveList<Segment, &Segment::node> SegmentList;
typedef IntrusiveList<Segment, &Segment::end_node> EndList;

struct Event {
    SegmentList start_segments;
    EndList end_segments;
};

void add_intersections_as_event_points(
    std::map<Point, Event> &event_queue,
    Points &tmp_intersections,
    const Point &event_point,
    const Segment &seg0,
    const Segment &seg1
){
    tmp_intersections.clear();
    find_intersections_two_segments(seg0.a, seg0.b, seg1.a, seg1.b, tmp_intersections);

    for (Point intersection : tmp_intersections){
        if (intersection > event_point){
            event_queue[intersection];
        }
    }
}

struct SweepSegment : Segment {
    SegmentList parallel_segments;

    SweepSegment(const Point &a, const Point &b): Segment(a, b){}

    SweepSegment(const SweepSegment &s): Segment(s.a, s.b){
        SweepSegment &todo_remove_ugly_hack = *(SweepSegment*)&s;
        parallel_segments.steal(todo_remove_ugly_hack.parallel_segments);
    }

    SweepSegment& operator = (const SweepSegment &s){
        a = s.a;
        b = s.b;
        return *this;
    }

    void add(Segment &seg){
        a = std::min(a, seg.a);
        b = std::max(b, seg.b);

        parallel_segments.push_back(seg);
    }
};


template <typename INTERSECTION_CALLBACK>
void find_intersections_sweepline(Segments &segments, INTERSECTION_CALLBACK &callback){
    std::map<Point, Event> event_queue;
    Points tmp_intersections;

    // Find slope larger than all other slopes to use as sentinel value
    Fraction max_slope(0);
    for (Segment &seg : segments){
        if (seg.b < seg.a){
            std::swap(seg.a, seg.b);
        }

        if (!seg.is_vertical()){
            Fraction abs_slope = abs(seg.slope());
            if (abs_slope > max_slope) max_slope = abs_slope;
        }

        event_queue[seg.a].start_segments.push_back(seg);
    }
    max_slope += 1;

    Point event_point{0, 0};
    SweepKey get_sweep_key(event_point, max_slope);
    SegmentComparator segment_comparator(get_sweep_key);
    std::set<SweepSegment, SegmentComparator> sweepline(segment_comparator);

    std::vector<const Segment*> intersecting_segments;

    size_t max_sweepline_size = 0;
    while (!event_queue.empty()){
        // Get new event point
        const auto &it_event = event_queue.begin();
        event_point = it_event->first;

        // Insert new segments starting at event point
        SegmentList &new_segments = it_event->second.start_segments;

        while (!new_segments.empty()){
            Segment &actual_seg = *new_segments.begin();
            new_segments.pop_front();

            if (actual_seg.b > event_point){
                event_queue[actual_seg.b].end_segments.push_back(actual_seg);
            }

            std::set<SweepSegment>::iterator it;

            // Merge with parallel segment if exists
            std::set<SweepSegment>::iterator it_lower_bound = sweepline.lower_bound(SweepSegment(actual_seg.a, actual_seg.b));
            if (it_lower_bound != sweepline.end() && get_sweep_key(*it_lower_bound) == get_sweep_key(actual_seg)){
                // TODO don't remove const somehow
                SweepSegment &seg2 = *(SweepSegment*)&(*it_lower_bound);

                seg2.add(actual_seg);

                it = it_lower_bound;
            }else{
                it = sweepline.emplace(actual_seg.a, actual_seg.b).first;

                // TODO don't remove const somehow
                SweepSegment &seg2 = *(SweepSegment*)&(*it);
                seg2.add(actual_seg);
            }

            // Check segment below and above newly inserted segment for intersections
            if (it != sweepline.begin()){
                add_intersections_as_event_points(event_queue, tmp_intersections, event_point, *std::prev(it), *it);
            }

            if (std::next(it) != sweepline.end()){
                add_intersections_as_event_points(event_queue, tmp_intersections, event_point, *it, *std::next(it));
            }
        }

        max_sweepline_size = std::max(max_sweepline_size, sweepline.size());

        // Find segments going through event_point
        Segment lower_segment{event_point, event_point + Point{0, 1}};
        Segment upper_segment{event_point + Point{0, 1}, event_point};

        std::set<SweepSegment>::iterator begin = sweepline.lower_bound(SweepSegment(lower_segment.a, lower_segment.b));

        // Equivalent to finding the upper bound:
        // std::set<Segment>::iterator end = sweepline.upper_bound(upper_segment);
        std::set<SweepSegment>::iterator end = begin;
        while (end != sweepline.end() && !segment_comparator(upper_segment, *end)) ++end;

        //t.stop("find bounds");

        std::set<SweepSegment>::iterator prev = begin != sweepline.begin() ? std::prev(begin) : sweepline.end();

        std::vector<SweepSegment> merged_intersecting_segments(begin, end);

        intersecting_segments.clear();
        size_t num_intersecting_segments = 0;
        for (const SweepSegment &merged_segment : merged_intersecting_segments){
            //num_intersecting_segments += merged_segment.indices.size();
            for (const Segment &segment : merged_segment.parallel_segments){
                // Only used for counting
                (void)segment;
                num_intersecting_segments++;

                intersecting_segments.push_back(&segment);
            }
        }

        if (intersecting_segments.size() > 1){
            callback(event_point, intersecting_segments);
        }

        // TODO no re-add if only single segment and no ends to delete
        sweepline.erase(begin, end);

        // Indicate that SweepKey should be reversed for event_point segments
        get_sweep_key.after_event_point = true;

        for (Segment &seg : it_event->second.end_segments){
            SegmentList::erase_value(seg);
        }

        for (SweepSegment &seg : merged_intersecting_segments){
            if (seg.b > event_point && !seg.parallel_segments.empty()){
                // TODO use insert hint
                sweepline.insert(seg);
            }
        }

        // Reset SweepKey so it works correctly for later event_points
        get_sweep_key.after_event_point = false;

        // Check for new intersections above and below intersecting segments
        if (prev != sweepline.end() && std::next(prev) != sweepline.end()){
            add_intersections_as_event_points(event_queue, tmp_intersections, event_point, *prev, *std::next(prev));
        }

        if (end != sweepline.end() && end != sweepline.begin()){
            add_intersections_as_event_points(event_queue, tmp_intersections, event_point, *std::prev(end), *end);
        }

        event_queue.erase(it_event);
    }
}

struct IntersectionCallbackDiscardSegments {
    std::vector<Point> intersections;

    void operator () (
        const Point &intersection,
        const std::vector<const Segment*>&
    ){
        intersections.push_back(intersection);
    }
};

std::vector<Point> find_intersections_sweepline(Segments &segments){
    IntersectionCallbackDiscardSegments callback;

    find_intersections_sweepline(segments, callback);

    return callback.intersections;
}

import random
import subprocess
import collections
from fractions import Fraction

def dot(a, b):
    ax, ay = a
    bx, by = b
    return ax * bx + ay * by

def det(a, b):
    ax, ay = a
    bx, by = b
    return ax * by - ay * bx

def sub(a, b):
    ax, ay = a
    bx, by = b
    return (ax - bx, ay - by)

def find_intersections_two_segments(a, b, c, d):
    # Compute intersections between segments (a, b) and (c, d).
    # Endpoints are also considered as intersections.
    # There can be at most one intersection at each point.
    # For example, a == b == c == d only counts as one intersection.

    ba = sub(b, a)
    dc = sub(d, c)
    ca = sub(c, a)

    ba_det_dc = det(ba, dc)
    ca_det_dc = det(ca, dc)
    ca_det_ba = det(ca, ba)

    # If segments are parallel
    if ba_det_dc == 0:
        # If parallel segments are on same line
        if ca_det_ba == 0 and ca_det_dc == 0:
            ba2 = dot(ba, ba)
            dc2 = dot(dc, dc)

            # If a == b and c == d
            if ba2 == 0 and dc2 == 0:
                if a == c: return [a]
            # If a == b
            elif ba2 == 0:
                # Return `a` if `a` lies on `(c, d)`
                if 0 <= dot(dc, sub(a, c)) <= dc2: return [a]
            # If c == d
            elif dc2 == 0:
                # Return `c` if `c` lies on `(a, b)`
                if 0 <= dot(ba, sub(c, a)) <= ba2: return [c]
            else:
                intersections = set()

                if 0 <= dot(ba, sub(c, a)) <= ba2: intersections.add(c)
                if 0 <= dot(ba, sub(d, a)) <= ba2: intersections.add(d)
                if 0 <= dot(dc, sub(a, c)) <= dc2: intersections.add(a)
                if 0 <= dot(dc, sub(b, c)) <= dc2: intersections.add(b)

                return list(intersections)
    else:
        t = Fraction(ca_det_ba, ba_det_dc)
        s = Fraction(ca_det_dc, ba_det_dc)

        if t >= 0 and t <= 1 and s >= 0 and s <= 1:
            x = a[0] + s * ba[0]
            y = a[1] + s * ba[1]

            intersection = (x, y)

            return [intersection]

    return []

def make_random_segments(num_segments, max_x, max_y):
    segments = []

    for _ in range(num_segments):
        ax = random.randrange(max_x)
        ay = random.randrange(max_y)
        bx = random.randrange(max_x)
        by = random.randrange(max_y)

        a = (ax, ay)
        b = (bx, by)

        segment = (a, b)

        segments.append(segment)

    return segments

def find_intersections_naive(segments):
    result = collections.defaultdict(set)

    for i, (a, b) in enumerate(segments):
        for j in range(i + 1, len(segments)):
            c, d = segments[j]
            for s in find_intersections_two_segments(a, b, c, d):
                result[s].add(i)
                result[s].add(j)

    return {intersection: sorted(sorted(segments[i]) for i in indices)
        for intersection, indices in result.items()}

def find_intersections(segments):
    result = {}

    text_segments = "\n".join(f"{ax} {ay} {bx} {by}\n"
        for (ax, ay), (bx, by) in segments).encode("utf-8")
    process = subprocess.run(["./main"], input=text_segments, capture_output=True)
    for block in process.stdout.decode("utf-8").strip().split("\n\n")[1:]:
        lines = block.split("\n")

        name, sx, sy = lines[0].split()
        assert name == "Intersection"

        intersection = (Fraction(sx), Fraction(sy))

        intersecting_segments = []
        for line in lines[1:]:
            name, ax, ay, bx, by = line.split()
            assert name == "Segment"

            a = (int(ax), int(ay))
            b = (int(bx), int(by))

            segment = [a, b]

            segment.sort()

            intersecting_segments.append(segment)

        intersecting_segments.sort()

        assert intersection not in result

        result[intersection] = intersecting_segments

    return result

def test_simple(num_tests):
    testcases = [
        ([(0, 0), (0, 0), (0, 0), (0, 0)], [(0, 0)]),
        ([(0, 0), (0, 0), (0, 0), (1, 0)], [(0, 0)]),
        ([(0, 0), (0, 0), (1, 0), (0, 0)], [(0, 0)]),
        ([(0, 0), (0, 0), (1, 0), (1, 0)], []),
        ([(0, 0), (1, 0), (0, 0), (0, 0)], [(0, 0)]),
        ([(0, 0), (1, 0), (0, 0), (1, 0)], [(0, 0), (1, 0)]),
        ([(0, 0), (1, 0), (1, 0), (0, 0)], [(0, 0), (1, 0)]),
        ([(0, 0), (1, 0), (1, 0), (1, 0)], [(1, 0)]),
        ([(1, 0), (0, 0), (0, 0), (0, 0)], [(0, 0)]),
        ([(1, 0), (0, 0), (0, 0), (1, 0)], [(0, 0), (1, 0)]),
        ([(1, 0), (0, 0), (1, 0), (0, 0)], [(0, 0), (1, 0)]),
        ([(1, 0), (0, 0), (1, 0), (1, 0)], [(1, 0)]),
        ([(1, 0), (1, 0), (0, 0), (0, 0)], []),
        ([(1, 0), (1, 0), (0, 0), (1, 0)], [(1, 0)]),
        ([(1, 0), (1, 0), (1, 0), (0, 0)], [(1, 0)]),
        ([(1, 0), (1, 0), (1, 0), (1, 0)], [(1, 0)]),

        ([(0, 0), (2, 0), (0, 0), (0, 0)], [(0, 0)]),
        ([(0, 0), (2, 0), (1, 0), (1, 0)], [(1, 0)]),
        ([(0, 0), (2, 0), (2, 0), (2, 0)], [(2, 0)]),

        ([(0, 0), (2, 0), (0, 0), (0, 1)], [(0, 0)]),
        ([(0, 0), (2, 0), (1, 0), (1, 1)], [(1, 0)]),
        ([(0, 0), (2, 0), (2, 0), (2, 1)], [(2, 0)]),

        ([(0, 0), (2, 0), (0, 0), (1, 0)], [(0, 0), (1, 0)]),
        ([(0, 0), (2, 0), (1, 0), (2, 0)], [(1, 0), (2, 0)]),
        ([(0, 0), (2, 0), (0, 0), (2, 0)], [(0, 0), (2, 0)]),

        ([(2, 0), (2, 0), (1, 0), (1, 0)], []),
        ([(2, 0), (2, 0), (1, 0), (1, 1)], []),
        ([(2, 0), (2, 1), (1, 0), (1, 0)], []),

        ([(2, 0), (0, 2), (0, 0), (2, 2)], [(1, 1)]),

        ([(0, 0), (1, 1), (1, 1), (2, 2)], [(1, 1)]),

        ([(0, 0), (2, 2), (-1, -1), (1, 1)], [(0, 0), (1, 1)]),
        ([(0, 0), (2, 2), (0, 0), (1, 1)], [(0, 0), (1, 1)]),
        ([(0, 0), (2, 2), (1, 1), (2, 2)], [(1, 1), (2, 2)]),
        ([(0, 0), (2, 2), (0, 0), (2, 2)], [(0, 0), (2, 2)]),
        ([(0, 0), (2, 2), (0, 0), (3, 3)], [(0, 0), (2, 2)]),
        ([(0, 0), (2, 2), (-1, -1), (3, 3)], [(0, 0), (2, 2)]),

        ([(0, 3), (2, 3), (-1, -1), (-1, 4)], []),
        ([(0, 3), (2, 3), (0, -1), (0, 4)], [(0, 3)]),
        ([(0, 3), (2, 3), (1, -1), (1, 4)], [(1, 3)]),
        ([(0, 3), (2, 3), (2, -1), (2, 4)], [(2, 3)]),
        ([(0, 3), (2, 3), (3, -1), (3, 4)], []),

        ([(2, 3), (2, 3), (2, 3), (2, 3)], [(2, 3)]),
        ([(2, 3), (2, 3), (3, 4), (3, 4)], []),
        ([(0, 0), (1, 0), (2, 0), (2, 1)], []),
        ([(1, 1), (7, 3), (4 - 3 * 4, 2 - 3 * 5), (4 + 2 * 4, 2 + 2 * 5)], [(4, 2)]),
    ]

    for (a, b, c, d), expected_intersections in testcases:
        segments = [(a, b), (c, d)]

        expected_result = find_intersections_naive(segments)

        result = find_intersections(segments)

        assert sorted(expected_result.keys()) == expected_intersections
        assert sorted(result.keys()) == expected_intersections
        assert result == expected_result

        num_tests += 1
        print("Passed test", num_tests)

    return num_tests

def test_random(num_tests):
    for num_segments in range(100):
        segments = make_random_segments(
            num_segments=num_segments, max_x=10, max_y=10)

        expected_result = find_intersections_naive(segments)

        result = find_intersections(segments)

        assert result == expected_result

        num_tests += 1
        print(f"Passed test {num_tests}")

    return num_tests

def main():
    print(subprocess.check_output(["make"]).decode("utf-8"))

    random.seed(0)

    num_tests = 0
    num_tests = test_simple(num_tests)
    num_tests = test_random(num_tests)

    print(f"Passed all {num_tests} tessed")

if __name__ == "__main__":
    main()

import subprocess
import matplotlib.pyplot as plt

def main():
    print(subprocess.check_output(["make"]).decode("utf-8"))

    output = subprocess.check_output(["./benchmark"])

    x = []
    y = []
    for line in output.decode("utf-8").strip().split("\n"):
        print(line)
        num_segments, _, num_intersections, _, elapsed_time, _ = line.split()
        x.append(int(num_segments))
        y.append(float(elapsed_time))

    plt.plot(x, y, 'o-')
    plt.title("Time for sweepline to find $n$ intersections of $n$ segments")
    plt.xlabel("Number of segments")
    plt.ylabel("Time [seconds]")
    plt.savefig("benchmark.png")
    plt.show()

if __name__ == "__main__":
    main()

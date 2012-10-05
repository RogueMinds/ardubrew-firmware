# Examples

## Usage

Display the graph script's usage information.

    ./graph.sh -h

    OPTIONS:
       -h      Show this message
       -d      Data set name (without the file extension)
       -b      Beginning date (defaults to the current timestamp)
       -e      Ending date (defaults to the beginning date + date range)
       -r      Date range (defaults to 84600 seconds)
       -u      Upper temperature reading (defautls to 85)
       -l      Lower temperature reading (defaults to 55)
       -a      Update unsynchronized timestamps
       -v      Verbose

Generate a graph from an incubator data set.

    ./graph.sh -d set-incubator -b 1313731650 -r 30000 -u 85 -l 80

![RRD Graph 1](https://raw.github.com/rudisimo/arduino-temp-controller/master/examples/.images/set-incubator.png)

Generate a graph from a cooler data set. The **-a** flag is needed
when using a non-timestamped log.

    ./graph.sh -d set-cooler -b 1349316000 -r 46250 -u 66 -l 60 -a

![RRD Graph 2](https://raw.github.com/rudisimo/arduino-temp-controller/master/examples/.images/set-cooler.png)


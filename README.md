# Shortest Flight Path

Simple Program that uses data from https://openflights.org/data.html to find the shortest path between two airports, using how ever many layovers as needed.

For sanitizing data provided by the CSV database, the information is first separated and outputted into a formatted XML file.

## How to Use

The command line program is simple. Just enter the IATA code of the start and ending airports, and an itinerary of the shortest path (not considering factors like cost or layover time) is outputted.

- Example showing flight from IBZ to IVC

![Image showing how program works](https://cdn.discordapp.com/attachments/325800539910832128/453455010244591616/qtcreator_process_stub_2018-06-05_00-05-57.png "ExampleImage")


- Also has error handling for several cases

![Image showing error messages](https://cdn.discordapp.com/attachments/325800539910832128/453457758893637633/qtcreator_process_stub_2018-06-05_00-05-02.png "ExampleError")

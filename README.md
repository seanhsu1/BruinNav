# BruinNav
Uses A* algorithm to find an optimum route between two attractions listed in the validlocs.txt file.

If the executable built from this file and the other .cpp files you write is named BruinNav (or BruinNav.exe on Windows), then you can run it with

./BruinNav theMapDataFileName "Start Attraction" "End Attraction"

to get the turn-by-turn instructions a user wants to see, or

./BruinNav theMapDataFileName "Start Attraction" "End Attraction" -raw

to see the sequence of NavSegments produced by Navigator::navigate()
(Under Windows, say "BruinNav" instead of "./BruinNav")

For example, with the mapdata.txt file we supplied you,

./BruinNav mapdata.txt "Harvard-Westlake Middle School" "GreyStone Mansion"


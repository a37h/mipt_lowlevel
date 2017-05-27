Unfinished parts:

1. If server has >1 workers at the beggining, then if one of them dies
(disconnects) then game just freezes but server and other workers will be sleeping, not exiting.

1.2 If server has >1 workers at the begging, one disconnects and another one tryes to take his place it
doesn't going to work out well, because server is sleeping.

2. Gliders aren't working as they has to. Probably some mistakes in calculating algorithm.

3. Some exceptions and errors aren't processed so you can get some user-not-friendly messages
 in the terminal window.
== Description ==
This is a gameboy advance game I wrote where you are a blue block and run away from red blocks. The red blocks chase you and you run away. Try to get to the escape window at the other side of the screen. 

== Running it ==
Open project1-final.gba using vba.exe. Press enter to start the game and use the arrow keys to move. 

== Source Code ==
This is one of the first C projects I ever did, so it ain't real pretty. It uses some fun data structures like linked lists, and it uses DMA to load images into memory quickly. You'll notice that as you advance through the levels that they get slower and slower. This is because I made a mistake: I used DMA too much and the performance of the gameboy starts going down quick. So it accidentally gets easier as you go haha.
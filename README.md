# MeadOnline

The app can be found here:
https://polar-citadel-83608.herokuapp.com/

This project has three major components
1. A NodeMCU that toggles power to a chest freezer to maintain the internal temperature within the optimal range for yeast fermentation.
That NodeMCU outputs the temperature data via HTTP requests to my back end.

2. The Express server on the back end receives these requests and stores the data in a Heroku postgres database

3. The front end uses React to display each batch's data in a graph and allow the user to download the data for further processing.


The project can easily scale to allow other people to save there data with the app and could be easily expanded to include more sensors.

This is my first Node.js project, and I welcome any constructive feedback!
# Connection

Identify client: 
{ "login" : <id string> }

Examples:
{ "login": "Chambord" }
{ "login": "Studio" }
{ "login": "Cindy" }

# Events

## Proximity

{ "proximity": <floating point cm, 0 means no presence> >

Examples:

// this is the default, also used to detect people leaving
{ "proximity": 0 }
// 23.5 cm
{ "proximity": 23.5 }
// Use this for on/off type proximity (presence)
{ "proximity": 1 }


## Knocking

o knock
o delay + knock
o knock pattern: starting with an optional delay, then a series of delays

Examples:
No initial delay: knock-knock-knock--knock
{ "knockpattern" : [ 0, 500, 500, 1000 ] }

Initial delay: -knock-knock--knock
{ "knockpattern" : [ 500, 500, 1000 ] }

Correct shave&haircut answer:
{ "knockpattern" : [ 1000, 500 ] }

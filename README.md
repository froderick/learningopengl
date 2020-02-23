# learn opengl

This repo documents my journey through [learnopengl.com](https://learnopengl.com/)'s excellent OpenGL tutorials. I'm mostly coming from a place of curiosity as to how lower-level graphics APIs work, but I am interested in making some simple 2d and 3d games as well. We'll see how it goes.

## getting started

First up, I couldn't get anything to work well in the getting started section of learnopengl.com. I ended up getting glfw and glew working on macos via the following blog, which was exactly what I needed: http://antongerdelan.net/opengl/hellotriangle.html. I had to install the following to make it work:

```
brew install glfw glew glm
```

At this point I had a purple triangle on the screen, yay.

/*

 transform -> relative position
 children -> holds references to child entities
 collides -> fires collision events
 health -> has hp, can receive damage, loses hp on damage, destroys when hp=0
 applies-collision-damage -> applies damage to things that take collision damage
 takes-collision-damage -> takes damage when colliding with things that apply collision damage
 absorbs-damage
 expires -> self-destructs after time
 power-up -> specifies the type of power up this is

 applies-collision-child -> on collision, adds a child entity to the thing it collides with

 */

// TODO: these components are too concrete, there should be more decomposed things like
//  - Transform - x/y coordinates + rotation + optionally relative to another entity
//  - Collidable - local x/y/width/height coordinates, subject to transformation before eval
//  - Velocity - x/y movement vectors
//  - ZigZagMovement
//  - SpiralMovement
//  - RelativeMovement
// Velocity, AINav, etc

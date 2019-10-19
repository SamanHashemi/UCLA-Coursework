window.Cube = window.classes.Cube =
class Cube extends Shape                 // Here's a complete, working example of a Shape subclass.  It is a blueprint for a cube.
  { constructor()
      { super( "positions", "normals" ); // Name the values we'll define per each vertex.  They'll have positions and normals.

        // First, specify the vertex positions -- just a bunch of points that exist at the corners of an imaginary cube.
        this.positions.push( ...Vec.cast( [-1,-1,-1], [1,-1,-1], [-1,-1,1], [1,-1,1], [1,1,-1],  [-1,1,-1],  [1,1,1],  [-1,1,1],
                                          [-1,-1,-1], [-1,-1,1], [-1,1,-1], [-1,1,1], [1,-1,1],  [1,-1,-1],  [1,1,1],  [1,1,-1],
                                          [-1,-1,1],  [1,-1,1],  [-1,1,1],  [1,1,1], [1,-1,-1], [-1,-1,-1], [1,1,-1], [-1,1,-1] ) );
        // Supply vectors that point away from eace face of the cube.  They should match up with the points in the above list
        // Normal vectors are needed so the graphics engine can know if the shape is pointed at light or not, and color it accordingly.
        this.normals.push(   ...Vec.cast( [0,-1,0], [0,-1,0], [0,-1,0], [0,-1,0], [0,1,0], [0,1,0], [0,1,0], [0,1,0], [-1,0,0], [-1,0,0],
                                          [-1,0,0], [-1,0,0], [1,0,0],  [1,0,0],  [1,0,0], [1,0,0], [0,0,1], [0,0,1], [0,0,1],   [0,0,1],
                                          [0,0,-1], [0,0,-1], [0,0,-1], [0,0,-1] ) );

                 // Those two lists, positions and normals, fully describe the "vertices".  What's the "i"th vertex?  Simply the combined
                 // data you get if you look up index "i" of both lists above -- a position and a normal vector, together.  Now let's
                 // tell it how to connect vertex entries into triangles.  Every three indices in this list makes one triangle:
        this.indices.push( 0, 1, 2, 1, 3, 2, 4, 5, 6, 5, 7, 6, 8, 9, 10, 9, 11, 10, 12, 13,
                          14, 13, 15, 14, 16, 17, 18, 17, 19, 18, 20, 21, 22, 21, 23, 22 );
        // It stinks to manage arrays this big.  Later we'll show code that generates these same cube vertices more automatically.
      }
  }

window.Transforms_Sandbox = window.classes.Transforms_Sandbox =
class Transforms_Sandbox extends Tutorial_Animation   // This subclass of some other Scene overrides the display() function.  By only
{ display( graphics_state )                           // exposing that one function, which draws everything, this creates a very small code
                                                      // sandbox for editing a simple scene, and for experimenting with matrix transforms.
    { let model_transform = Mat4.identity();      // Variable model_transform will be a temporary matrix that helps us draw most shapes.
                                                  // It starts over as the identity every single frame - coordinate axes at the origin.
      graphics_state.lights = this.lights;        // Use the lights stored in this.lights.
      /**********************************
      Start coding down here!!!!
      **********************************/         // From here on down it's just some example shapes drawn for you -- freely replace them
                                                  // with your own!  Notice the usage of the functions translation(), scale(), and rotation()
                                                  // to generate matrices, and the functions times(), which generates products of matrices.

      const blue = Color.of( 0,0,1,1 ), yellow = Color.of( 1,1,0,1 );
      model_transform = model_transform.times( Mat4.translation([ 0, 3, 20 ]) );
      this.shapes.box.draw( graphics_state, model_transform, this.plastic.override({ color: yellow }) );   // Draw the top box

      const t = this.t = graphics_state.animation_time/1000;     // Find how much time has passed in seconds, and use that to place shapes.

      model_transform = model_transform.times( Mat4.translation([ 0, -2, 0 ]) );  // Tweak our coordinate system downward for the next shape.
      this.shapes.ball.draw( graphics_state, model_transform, this.plastic.override({ color: blue }) );    // Draw the ball.

      if( !this.hover )     // The first line below won't execute if the button on the page has been toggled:
        model_transform = model_transform.times( Mat4.rotation( t, Vec.of( 0,1,0 ) ) )  // Spin our coordinate frame as a function of time.
      model_transform   = model_transform.times( Mat4.rotation( 1, Vec.of( 0,0,1 ) ) )  // Rotate another axis by a constant value.
                                         .times( Mat4.scale      ([ 1,   2, 1 ]) )      // Stretch the coordinate frame.
                                         .times( Mat4.translation([ 0,-1.5, 0 ]) );     // Translate down enough for the two volumes to miss.
      this.shapes.box.draw( graphics_state, model_transform, this.plastic.override({ color: yellow }) );   // Draw the bottom box.
    }
}

window.Cube_Outline = window.classes.Cube_Outline =
class Cube_Outline extends Shape
  { constructor()
      { super( "positions", "colors" ); // Name the values we'll define per each vertex.
        this.positions.push( ...Vec.cast( [1,1,-1], [1,-1,-1], [-1,-1,1], [1,-1,1], [1,1,-1],  [-1,1,-1],  [1,1,1],  [-1,1,1],
                                          [-1,-1,-1], [-1,-1,1], [-1,1,-1], [-1,1,1], [1,-1,1],  [1,-1,-1],  [1,1,1],  [1,1,-1],
                                          [-1,-1,1],  [-1,1,1],  [1,-1,1],  [1,1,1], [1,-1,-1], [-1,-1,-1], [-1,1,-1], [-1,-1,-1] ) );

                                      
        this.colors = [Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),
        Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),
        Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),
        Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),
        Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),
        Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1),Color.of(255,255,255,1)];
       
       this.indexed = false;       // Do this so we won't need to define "this.indices".

      }
  }

window.Cube_Single_Strip = window.classes.Cube_Single_Strip =
class Cube_Single_Strip extends Shape
  { constructor()
      { super( "positions", "normals" );

        // TODO (Extra credit part I)
      }
  }

window.Assignment_One_Scene = window.classes.Assignment_One_Scene =
class Assignment_One_Scene extends Scene_Component
  { constructor( context, control_box )     // The scene begins by requesting the camera, shapes, and materials it will need.
      { super(   context, control_box );    // First, include a secondary Scene that provides movement controls:
        if( !context.globals.has_controls )
          context.register_scene_component( new Movement_Controls( context, control_box.parentElement.insertCell() ) );
        this.sit_still = false; // Moving to start
        this.outline = false;
        const r = context.width/context.height;
        context.globals.graphics_state.    camera_transform = Mat4.translation([ 5,-10,-30 ]);  // Locate the camera here (inverted matrix).
        context.globals.graphics_state.projection_transform = Mat4.perspective( Math.PI/4, r, .1, 1000 );

        const shapes = { 'box': new Cube(),               // At the beginning of our program, load one of each of these shape
                       'strip': new Cube_Single_Strip(),  // definitions onto the GPU.  NOTE:  Only do this ONCE per shape
                     'outline': new Cube_Outline() }      // design.  Once you've told the GPU what the design of a cube is,
        this.submit_shapes( context, shapes );            // it would be redundant to tell it again.  You should just re-use
                                                          // the one called "box" more than once in display() to draw
                                                          // multiple cubes.  Don't define more than one blueprint for the
                                                          // same thing here.

                                                          // Make some Material objects available to you:
        this.clay   = context.get_instance( Phong_Shader ).material( Color.of( .9,.5,.9, 1 ), { ambient: .4, diffusivity: .4 } );
        this.white  = context.get_instance( Basic_Shader ).material();
        this.plastic = this.clay.override({ specularity: .6 });

        this.lights = [ new Light( Vec.of( 0,5,5,1 ), Color.of( 1, .4, 1, 1 ), 100000)];
        this.set_colors();
      }
      
    set_colors() {
      
      // Store Colors
      this.colors = [ Color.of(0,1,1,1), Color.of( 0,0,1,1 ), Color.of(0,1,0,1), Color.of(1,1,0,1),
      Color.of(1,1,1,1),Color.of(2,1,1,1),Color.of(2,2,1,1)];

      // Randomize
      this.colors.sort(function() { return 0.5 - Math.random() });
      }

    make_control_panel()             // Draw the scene's buttons, setup their actions and keyboard shortcuts, and monitor live measurements.
      { this.key_triggered_button( "Change Colors", [ "c" ], this.set_colors );    // Add a button for controlling the scene.
        this.key_triggered_button( "Outline",       [ "o" ], () => { this.outline = !this.outline; } ); // Toggle outline
        this.key_triggered_button( "Sit still",     [ "m" ], () => { this.sit_still = !this.sit_still; } ); // Toggle Movement flag
      }
    draw_box( graphics_state, model_transform, box_num)
      {
        model_transform = model_transform.times( Mat4.translation([ 1,1, 0 ]) ); // Anchor

        // Toggle movement
        if(!this.sit_still)
          model_transform = model_transform.times( Mat4.rotation( -.0625+.0625*Math.sin(-0.4*Math.PI*6*graphics_state.animation_time/500) , Vec.of( 0,0,1 ) ) )  
        else
          model_transform = model_transform.times( Mat4.rotation( (-.0625*2) , Vec.of( 0,0,1 ) ) ) ;
        
        model_transform = model_transform.times( Mat4.translation([ -1,1, 0 ]) ); // Re-anchor
        

        // Colored or outlined
        if(this.outline){
          this.shapes.outline.draw( graphics_state, model_transform,this.white,"LINES" );
        }
         else{
           this.shapes.box.draw( graphics_state, model_transform,this.plastic.override({ color: this.colors[box_num] }) );
         }

        return model_transform;
      }
    display( graphics_state )
      { graphics_state.lights = this.lights; // Use the lights stored in this.lights.      
        
        // Set the model_transform to to be a unit box
        let model_transform = Mat4.identity();

        //Add Basis block
        model_transform = model_transform.times( Mat4.translation([ 0,-1, 0 ]) );
        this.shapes.box.draw( graphics_state, model_transform,this.plastic.override({ color: Color.of(2,0,9,1) }) );
        
        // Add remaining blocks
        for(var i = 0; i < 7; i++)
          model_transform = this.draw_box(graphics_state, model_transform, i); 
      }
  }
const g = -9.8; //gravity
const mass = 0.2; //fixed mass of the ball


window.Main_Scene = window.classes.Main_Scene =
class Main_Scene extends Scene_Component
  { constructor( context, control_box )     // The scene begins by requesting the camera, shapes, and materials it will need.
      { super(   context, control_box );    // First, include a secondary Scene that provides movement controls:
      
        this.scoreBoard = new Scoreboard( context, control_box.parentElement.insertCell(), 0,0)
        if( !context.globals.has_controls   ) 
          context.register_scene_component( new Movement_Controls( context, control_box.parentElement.insertCell() ) ); 
          context.register_scene_component( this.scoreBoard )

        this.camera = Mat4.look_at( Vec.of( -28,2.9,0 ), Vec.of( 0,0,0 ), Vec.of( 0,1,0 ) );
        
        context.globals.graphics_state.camera_transform = this.camera

        const r = context.width/context.height;
        context.globals.graphics_state.projection_transform = Mat4.perspective( Math.PI/4, r, .1, 1000 );
        
        
        const shapes = { net: new Net(),
                         sphere: new Ball(4, 0, -2.5, 0, 0), //set x and y positions of ball
                         cylinder: new Cylindrical_Tube(4,100,[[0,1],[0,1]]),
                         cube: new Cube(),
                         field: new Cube(),
                         monkey: new Monkey("assets/MonkeyOBJ.obj"),
                         boundary: new Boundary(),
                         bollard: new Bollard(),
                        }

        this.submit_shapes( context, shapes );

        this.materials =
          { 
            field:   context.get_instance( Fake_Bump_Map ).material( Color.of(0,0,0,1), 
              {texture: context.get_instance("assets/sand_smooth.jpg", true), ambient: .3, diffusivity: .5, specularity: .5}), //try bumpmapping
            net:     context.get_instance( Phong_Shader ).material( Color.of( 0,0,0,1 ), { ambient:1 }),
            sphere:  context.get_instance( Phong_Shader ).material( Color.of( 0.567,0.697,0.912,1 ), { ambient:0.5 }),
            ball:    context.get_instance( Texture_Rotate ).material( Color.of(0,0,0,1), 
              {texture: context.get_instance("assets/volleyball.png", true), ambient: 1}),
            monkey:  context.get_instance( Fake_Bump_Map ).material( Color.of(0.519,0.834,1,1), 
              { ambient:0.7, diffusivity:0.3, specularity: 0.5  }),
            boundary: context.get_instance( Phong_Shader ).material( Color.of(1,1,1,1), {ambient:1}),
            bollard: context.get_instance( Phong_Shader ).material( Color.of(0.980,0.010,0.038,1), {ambient: 0.1}),
            //palm:  context.get_instance( Fake_Bump_Map ).material( Color.of(0.750,0.481,0.1241,1), 
              //{ ambient:0.8, diffusivity:0.3, specularity: 0.5  }),
          } //use nearest neighbor

        this.lights = [ new Light( Vec.of( -5,5,5,1 ), Color.of( 0,1,1,1 ), 100000 ) ];
     
        this.time_scale = 1;
        this.time_accumulator = 0; 
        this.dt = 1/20;
        this.t = 0;
        this.steps_taken = 0;

        this.left = false;
        this.right = false;
        this.cur_val = 0;

        this.monkey1_position = Mat4.identity().times( Mat4.translation([0, -3.5, -4.5]));

        this.monkey2_position = Mat4.identity().times( Mat4.translation([0, -3.5, 4.5])).times(Mat4.rotation(Math.PI, [0,1,0]))    
         

        this.initial_ball_position = Mat4.identity().times( Mat4.translation([0, 2, 0])).times( Mat4.scale([0.6,0.6,0.6]).times(Mat4.rotation(Math.PI/2, [0,1,0])))

        this.ball_position = Mat4.identity().times( Mat4.translation([0, 2, 0])).times( Mat4.scale([0.6,0.6,0.6]).times(Mat4.rotation(Math.PI/2, [0,1,0])))

        this.m1MoveL = false;    
        this.m1MoveR = false;
        this.m1MoveU = false;
        this.m1MoveD = false;
        this.m2MoveL = false;
        this.m2MoveR = false; 
        this.m2MoveU = false;
        this.m2MoveD = false;  


        
  
        this.physicsFlag = false;
        this.hitGround = true;       

       
        this.hardHitFlag = false;
      
        this.hitNet = false;
        this.animate = false;

        this.ball = new Body(this.shapes.sphere, this.materials.ball, Vec.of(1,1,1))
              .emplace( this.ball_position, Vec.of(0,0,0).times(3), 0 );    //entered this.ball_position here for consistency
        this.downFlag = true;   
        
        this.lastHit = false; //keep track of who hit it last

        this.orientation = false;   
        this.out_of_bounds = false;
                              
        this.m1ForceY = 30;
        this.m2ForceY = 30;  
        this.m1ForceX = 15;
        this.m2ForceX = 15;    
              
      }
    handleReset() {
        this.score_collision_detection();
        this.ball = new Body(this.shapes.sphere, this.materials.ball, Vec.of(1,1,1))
           .emplace( this.initial_ball_position ,Vec.of(0,0,0).times(3),0 ) ;
        this.animate = false;
        this.physicsFlag = false;
        this.out_of_bounds = false;
        this.ball.resetForce();
        this.m1ForceY = 30;
        this.m2ForceY = 30;  
        this.m1ForceX = 15;
        this.m2ForceX = 15; 
        this.monkey1_position = Mat4.identity().times( Mat4.translation([0, -3.5, -4.5]));

        this.monkey2_position = Mat4.identity().times( Mat4.translation([0, -3.5, 4.5])).times(Mat4.rotation(Math.PI, [0,1,0]))    
         
    }
    make_control_panel()
      { 
        this.key_triggered_button( "Move P1 Left", [ "q" ], () => {
          this.m1MoveL = !this.m1MoveL;       
        } );
        this.key_triggered_button( "Move P1 Right", [ "w" ], () => {
          this.m1MoveR = !this.m1MoveR;       
        } );
        
        /*this.key_triggered_button( "Move P1 Up", [ "w" ], () => {
          this.m1MoveU = !this.m1MoveU;       
        } );
        this.key_triggered_button( "Move P1 Down", [ "s" ], () => {
          this.m1MoveD = !this.m1MoveD;       
        } );*/


        this.key_triggered_button( "Move P2 Left", [ "o" ], () => {
          this.m2MoveL = !this.m2MoveL;       
        } );


        this.key_triggered_button( "Move P2 Right", [ "p" ], () => {
          this.m2MoveR = !this.m2MoveR;       
        } );
        
        /*this.key_triggered_button( "Move P1 Up", [ "o" ], () => {
          this.m2MoveU = !this.m2MoveU;       
        } );
        this.key_triggered_button( "Move P1 Down", [ "l" ], () => {
          this.m2MoveD = !this.m2MoveD;       
        } );*/


        this.new_line();

        this.key_triggered_button( "Drop ball", [ "d" ], () => { // Start game this way
          this.animate = !this.animate;
          this.physicsFlag = true;
        } );

        this.key_triggered_button( "Reset ball", [ "b" ], () => { // Start game this way
          this.handleReset();
        } );

        
        

        this.key_triggered_button( "Player 2 Toggle Hard Hit On (increase force)", ["k"], () => {
          this.ball.updateForce()
          if(this.ball.center[2] > 0 && !this.lastHit) {
            this.m2ForceY = 45;
            this.m2ForceX = 30;
          }
        })

        this.key_triggered_button( "Player 2 Toggle Hard Hit Off (decrease force)", ["l"], () => {
          this.ball.resetForce()
          if(this.ball.center[2] > 0 && !this.lastHit) {
            this.m2ForceY = 30;
            this.m2ForceX = 15;
          }
        })
        
        this.key_triggered_button( "Player 1 Toggle Hard Hit On (increase force)", ["a"], () => {
          this.ball.updateForce()
          if(this.ball.center[2] < 0  && this.lastHit) {
            this.m1ForceY = 45;
            this.m1ForceX = 30;
          }
        })

        this.key_triggered_button( "Player 1 Toggle Hard Hit Off (decrease force)", ["s"], () => {
          this.ball.resetForce()
          if(this.ball.center[2] < 0 && this.lastHit) {
            this.m1ForceY = 30;
            this.m1ForceX = 15;
          }
        })

      }

    
    handlePhysics(graphics_state) {
      let ax = 0, ay=0;
      if(this.ball.center[2] > 0) {
        ax = this.m2ForceX / mass; //a = F/m
        ay = this.m2ForceY / mass;
      }
      else {
        console.log(this.m2ForceY)
        ax = this.m1ForceX / mass; //a = F/m
        ay = this.m1ForceY / mass;
      }
      
      let dt = 1.5/100; //for smoother movement

      this.ball.velX = this.ball.velX + ax*dt;
      let x_distance = this.ball.velX * dt;


      this.ball.velY = this.ball.velY + ay*dt;
      let y_distance = this.ball.velY * dt;
      
      return {x: x_distance, y: y_distance};
      //amount moved in x direction 

    }

    //update the score based on what where the ball collided with the field
    score_collision_detection() {
      if(this.lastHit == false && this.hitNet) {
        this.scoreBoard.updateScore2();
      }
      else if(this.lastHit == true && this.hitNet){
        this.scoreBoard.updateScore1();
      }
      else if(this.lastHit == false && !this.out_of_bounds) {
        this.scoreBoard.updateScore1();
      }
      else if(this.lastHit == true && !this.out_of_bounds) {
        this.scoreBoard.updateScore2();
      }
    }
    move_ball(t, dt, y_upper, x_position, orientation,field_transform) {
      let temp_transform = this.ball.drawn_location;
      let move_x = 0;
      if(orientation == false) {
        move_x = -dt*8;
      }
      else {
        move_x = dt*8;
      }

      this.ball.drawn_location = temp_transform;
      if((this.ball.drawn_location[1][3] < y_upper && !this.downFlag)) { 
        this.ball.drawn_location = this.ball.drawn_location.times(Mat4.translation([0, dt*8, 0]))
        this.ball.drawn_location = this.ball.drawn_location.times(Mat4.translation([move_x, 0, 0])) 
        
      }
      else {
        this.ball.drawn_location = this.ball.drawn_location.times(Mat4.translation([move_x, 0, 0])) 
        this.ball.drawn_location = this.ball.drawn_location.times(Mat4.translation([0, -dt*8, 0]))
        this.downFlag = true;
      
      }

      this.update_state( this.dt, field_transform );  
      this.ball.center = this.ball.drawn_location.times( Vec.of( 0,0,0,1 ) ).to3();
//       this.ball.center = this.ball.center.plus( this.ball.linear_velocity.times( this.dt ) ); 
    }

    update_state( dt, field_transform ) { 
      let field_location = field_transform.times( Vec.of( 0,0,0,1 ) ).to3();
      this.ball.linear_velocity[1] += dt * g;                      // Gravity on Earth, where 1 unit in world space = 1 meter.
     
      if( this.ball.center[1] <= field_location[1] ) {
//          console.log("hitting field");
         this.ball.linear_velocity[1] *= -.8;  
         this.handleReset();
      }

      if(Math.abs(this.ball.center[2]) >= 14.25 ) {
//         console.log("out of bounds");
        this.out_of_bounds = true;
        if(this.lastHit) {
          this.scoreBoard.updateScore1();
        } else {
          this.scoreBoard.updateScore2();
        }
        this.handleReset();
      }

     }

    drawMonkeys(graphics_state, dt) {
       this.shapes.monkey.init_position(this.monkey1_position);
       if(this.shapes.monkey.collision_check(this.ball.drawn_location)) {

         this.ball.velX = 0;
         this.ball.velY = 0;
         this.downFlag = !this.downFlag;
         this.orientation = !this.orientation;
         this.lastHit = !this.lastHit; //true when monkey 1 hits it
         //this.m2HitLeft = false;
         //this.m2HitRight = false;
         this.z_direction_p1 = true;
         //The ball should fly over the net here
       }
       //Maybe need to set the movement variables to false? -> check later
       if(!this.m1MoveL && !this.m1MoveR) { //nothing
         this.shapes.monkey.draw( graphics_state, this.monkey1_position, this.materials.monkey)      
       }

       if(this.m1MoveL && !this.m1MoveR) {//left
         if(this.monkey1_position[2][3] >= -14.25) {
           this.monkey1_position = this.monkey1_position.times(Mat4.translation([0,0,dt*-25]));
           this.shapes.monkey.draw( graphics_state, this.monkey1_position, this.materials.monkey)
         } //make movement stop by detecting collision with white line
         this.m1MoveL = false;
       }

       if(this.m1MoveR && !this.m1MoveL) {//right
         if(this.monkey1_position[2][3] <= -1.33) {
          this.monkey1_position = this.monkey1_position.times(Mat4.translation([0,0,dt*25]));
          this.shapes.monkey.draw( graphics_state, this.monkey1_position, this.materials.monkey) 
         } 
         this.m1MoveR = false;
       }

       if(!this.m1MoveL && !this.m1MoveR && this.m1MoveU && !this.m1MoveD) {//up 
          if(this.monkey1_position[0][3] <= 6) {
            this.monkey1_position = this.monkey1_position.times(Mat4.translation([dt*25, 0, 0]));
            this.shapes.monkey.draw( graphics_state, this.monkey1_position, this.materials.monkey) 
          }
          this.m1MoveU = false;
       }

       if(!this.m1MoveL && !this.m1MoveR && !this.m1MoveU && this.m1MoveD) {//down
          if(this.monkey1_position[0][3] >= -6) {
            this.monkey1_position = this.monkey1_position.times(Mat4.translation([dt*-25, 0, 0]));
            this.shapes.monkey.draw( graphics_state, this.monkey1_position, this.materials.monkey)
          }   
          this.m1MoveD = false;
       }

       this.shapes.monkey.init_position(this.monkey2_position);
       if(this.shapes.monkey.collision_check(this.ball.drawn_location)) {
        this.ball.velX = 0;
        this.ball.velY = 0;
        this.downFlag = !this.downFlag;
        this.orientation = !this.orientation;
        this.lastHit = !this.lastHit; //false when monkey 2 hits it
       }

       if(!this.m2MoveL && !this.m2MoveR && !this.m2MoveU && !this.m2MoveD) {
         this.shapes.monkey.draw( graphics_state, this.monkey2_position, this.materials.monkey.override({color: Color.of(1.000,0.998,0.364,1)}))
       }

       if(this.m2MoveL && !this.m2MoveR) {
         if(this.monkey2_position[2][3] >= 1.33) {
          this.monkey2_position = this.monkey2_position.times(Mat4.translation([0,0,dt*25]));
          this.shapes.monkey.draw( graphics_state, this.monkey2_position, this.materials.monkey.override({color: Color.of(1.000,0.998,0.364,1)})) 
         }
         this.m2MoveL = false; //make sure the movement stops
       }

       if(!this.m2MoveL && this.m2MoveR) {
         if(this.monkey2_position[2][3] <= 14.25) {
          this.monkey2_position = this.monkey2_position.times(Mat4.translation([0, 0, dt*-25]));
          this.shapes.monkey.draw( graphics_state, this.monkey2_position, this.materials.monkey.override({color: Color.of(1.000,0.998,0.364,1)}))
         }
         this.m2MoveR = false;// work on smooth movement?
       }

       if(!this.m2MoveL && !this.m2MoveR && this.m2MoveU && !this.m2MoveD) {//up 
          if(this.monkey2_position[0][3] <= 6) {
            this.monkey2_position = this.monkey2_position.times(Mat4.translation([-dt*25, 0, 0]));
            this.shapes.monkey.draw( graphics_state, this.monkey2_position, this.materials.monkey)
          }
          this.m2MoveU = false;
       }

       if(!this.m2MoveL && !this.m2MoveR && !this.m2MoveU && this.m2MoveD) {//down 
          if(this.monkey2_position[0][3] >= -6) {
            this.monkey2_position = this.monkey2_position.times(Mat4.translation([dt*25, 0, 0]));
            this.shapes.monkey.draw( graphics_state, this.monkey2_position, this.materials.monkey)
          }
          this.m2MoveD = false;
       }

    }

    display( graphics_state )
      { graphics_state.lights = this.lights;        // Use the lights stored in this.lights.
        const t = graphics_state.animation_time / 1000, dt = graphics_state.animation_delta_time / 1000; //delta time = time past since last frame
        
        let model_transform = Mat4.identity();
        let field_transform = Mat4.identity();
        let boundary_transform = Mat4.identity();
        let tree_transform = Mat4.identity();

        field_transform = field_transform.times(Mat4.rotation( Math.PI/2, Vec.of(1,0, 0))).times( Mat4.rotation(Math.PI/2, Vec.of(0,0,1)));

        field_transform = field_transform.times(Mat4.translation([0, 0, 5])).times(Mat4.scale([23,15,0.05]))
        this.shapes.field.draw(graphics_state, field_transform, this.materials.field.override({color: Color.of(0.994,1.000,0.449, 1)}))


        boundary_transform = boundary_transform.times( Mat4.translation( [0, -4.95, 0])) //give some leeway
        this.shapes.boundary.draw(graphics_state, boundary_transform, this.materials.boundary)
        // Drawing the net
        this.shapes.net.draw(graphics_state, model_transform, this.materials.net);
        
        
        this.shapes.bollard.draw(graphics_state, tree_transform, this.materials.bollard);

        // Drawing the ball
        let positions = {};
        if(this.physicsFlag) {
          positions = this.handlePhysics(graphics_state)
        }
        
        // Drop ball
        if(this.animate)
           this.move_ball(t, dt, positions.y, 0, this.orientation,field_transform) 

        this.drawMonkeys( graphics_state,dt );

        this.ball.shape.draw( graphics_state, this.ball.drawn_location, this.ball.material );
        
        if(this.shapes.net.check_collision(this.ball.drawn_location)) {
          this.hitNet = true;
          this.handleReset();
        }
      }
  }

window.Ball = window.classes.Ball =
class Ball extends Shape  
{                                      
  constructor( max_subdivisions, x, y, dx, dy )       
    { super( "positions", "normals", "texture_coords" );                     
      this.positions.push( ...Vec.cast( [ 0, 0, -1 ], [ 0, .9428, .3333 ], [ -.8165, -.4714, .3333 ], [ .8165, -.4714, .3333 ] ) );
      
      this.subdivideTriangle( 0, 1, 2, max_subdivisions);  // Begin recursion.
      this.subdivideTriangle( 3, 2, 1, max_subdivisions);
      this.subdivideTriangle( 1, 0, 3, max_subdivisions);
      this.subdivideTriangle( 0, 2, 3, max_subdivisions); 
      
      for( let p of this.positions )
        { this.normals.push( p.copy() );                 // Each point has a normal vector that simply goes to the point from the origin.

                                                         // Textures are tricky.  A Subdivision sphere has no straight seams to which image 
                                                         // edges in UV space can be mapped.  The only way to avoid artifacts is to smoothly                                                          
          this.texture_coords.push(                      // wrap & unwrap the image in reverse - displaying the texture twice on the sphere.
                                 Vec.of( Math.asin( p[0]/Math.PI ) + .5, Math.asin( p[1]/Math.PI ) + .5 ) ) }
                           
    }
  
  subdivideTriangle( a, b, c, count )   // Recurse through each level of detail by splitting triangle (a,b,c) into four smaller ones.
    { 
      if( count <= 0) { this.indices.push(a,b,c); return; }  // Base case of recursion - we've hit the finest level of detail we want.
                  
      var ab_vert = this.positions[a].mix( this.positions[b], 0.5).normalized(),     // We're not at the base case.  So, build 3 new
          ac_vert = this.positions[a].mix( this.positions[c], 0.5).normalized(),     // vertices at midpoints, and extrude them out to
          bc_vert = this.positions[b].mix( this.positions[c], 0.5).normalized();     // touch the unit sphere (length 1).
            
      var ab = this.positions.push( ab_vert ) - 1,      // Here, push() returns the indices of the three new vertices (plus one).
          ac = this.positions.push( ac_vert ) - 1,  
          bc = this.positions.push( bc_vert ) - 1;  
      
      this.subdivideTriangle( a, ab, ac,  count - 1 );          // Recurse on four smaller triangles, and we're done.  Skipping every
      this.subdivideTriangle( ab, b, bc,  count - 1 );          // fourth vertex index in our list takes you down one level of detail,
      this.subdivideTriangle( ac, bc, c,  count - 1 );          // and so on, due to the way we're building it.
      this.subdivideTriangle( ab, bc, ac, count - 1 );
    }
}

class Body          // Store and update the properties of a 3D body that increntally moves from its previous place due to velocities.
{ constructor(               shape, material, size )
    { Object.assign( this, { shape, material, size } ) }
  emplace( location_matrix, linear_velocity, angular_velocity, spin_axis = Vec.of(0,0,0).randomized(1).normalized() )
    { this.center   = location_matrix.times( Vec.of( 0,0,0,1 ) ).to3();
      this.rotation = Mat4.translation( this.center.times( -1 ) ).times( location_matrix );
      this.previous = { center: this.center.copy(), rotation: this.rotation.copy() };
      this.drawn_location = location_matrix;           
      this.forceX = 15;
      this.forceY = 30;
      this.velX = 0;
      this.velY = 0;                           // This gets replaced with an interpolated quantity.
      return Object.assign( this, { linear_velocity, angular_velocity, spin_axis } )
       //Physics 
      
    }
  updateForce() {
//     console.log("forceY",forceY);
    this.forceX = 40;
    this.forceY = 45;
  }
  resetForce() {
    this.forceX = 15;
    this.forceY = 30;
  }
}

class Monkey extends Shape          // A versatile standalone Shape that imports all its arrays' data from an .obj 3D model file.
{ constructor( filename )
    { super( "positions", "normals", "texture_coords" );
      this.position = {
        x: 0,
        y:0,
        z: 0
      }
      this.load_file( filename );      // Begin downloading the mesh. Once that completes, return control to our parse_into_mesh function.
    }
  load_file( filename )
      { return fetch( filename )       // Request the external file and wait for it to load.
          .then( response =>
            { if ( response.ok )  return Promise.resolve( response.text() )
              else                return Promise.reject ( response.status )
            })
          .then( obj_file_contents => this.parse_into_mesh( obj_file_contents ) )
          .catch( error => { this.copy_onto_graphics_card( this.gl ); } )                     // Failure mode:  Loads an empty shape.
      }
  init_position(matrix) {
    this.position = {
      x: matrix[0][3], //[0][3] represents the coordinates we need I believe, please check later
      y: matrix[1][3],
      z: matrix[2][3] 
    }
    return this.position;
  }

  //Simple collision detection for the ball -> check if the ball is within a certain radius -> we did not want sphere
  collision_check(ball) {
     //Calculate the distances between the ball and the position of the monkey
     let distanceX = Math.abs(ball[0][3] - this.position.x)
     let distanceY = Math.abs(ball[1][3] - this.position.y)
     let distanceZ = Math.abs(ball[2][3] - this.position.z) //distances for collision detection


     if(distanceX > 0.6 || distanceY > 2.15 || distanceZ > 0.27) { //half of the height of the monkey
       return false; 
     }
    
     if(distanceX <= 0.6 && distanceY <= 2.15 && distanceZ <= 0.27) { //account for monkey height and length
       return true;
     }
     return false; 
  }
  parse_into_mesh( data )                                         
    { var verts = [], vertNormals = [], textures = [], unpacked = {};   

      unpacked.verts = [];        unpacked.norms = [];    unpacked.textures = [];
      unpacked.hashindices = {};  unpacked.indices = [];  unpacked.index = 0;

      var lines = data.split('\n');

      var VERTEX_RE = /^v\s/;    var NORMAL_RE = /^vn\s/;    var TEXTURE_RE = /^vt\s/;
      var FACE_RE = /^f\s/;      var WHITESPACE_RE = /\s+/;

      for (var i = 0; i < lines.length; i++) {
        var line = lines[i].trim();
        var elements = line.split(WHITESPACE_RE);
        elements.shift();

        if      (VERTEX_RE.test(line))   verts.push.apply(verts, elements);
        else if (NORMAL_RE.test(line))   vertNormals.push.apply(vertNormals, elements);
        else if (TEXTURE_RE.test(line))  textures.push.apply(textures, elements);
        else if (FACE_RE.test(line)) {
          var quad = false;
          for (var j = 0, eleLen = elements.length; j < eleLen; j++)
          {
              if(j === 3 && !quad) {  j = 2;  quad = true;  }
              if(elements[j] in unpacked.hashindices) 
                  unpacked.indices.push(unpacked.hashindices[elements[j]]);
              else
              {
                  var vertex = elements[ j ].split( '/' );

                  unpacked.verts.push(+verts[(vertex[0] - 1) * 3 + 0]);   unpacked.verts.push(+verts[(vertex[0] - 1) * 3 + 1]);   
                  unpacked.verts.push(+verts[(vertex[0] - 1) * 3 + 2]);
                  
                  if (textures.length) 
                    {   unpacked.textures.push(+textures[( (vertex[1] - 1)||vertex[0]) * 2 + 0]);
                        unpacked.textures.push(+textures[( (vertex[1] - 1)||vertex[0]) * 2 + 1]);  }
                  
                  unpacked.norms.push(+vertNormals[( (vertex[2] - 1)||vertex[0]) * 3 + 0]);
                  unpacked.norms.push(+vertNormals[( (vertex[2] - 1)||vertex[0]) * 3 + 1]);
                  unpacked.norms.push(+vertNormals[( (vertex[2] - 1)||vertex[0]) * 3 + 2]);
                  
                  unpacked.hashindices[elements[j]] = unpacked.index;
                  unpacked.indices.push(unpacked.index);
                  unpacked.index += 1;
              }
              if(j === 3 && quad)   unpacked.indices.push( unpacked.hashindices[elements[0]]);
          }
        }
      }

      for( var j = 0; j < unpacked.verts.length/3; j++ )
      {
        this.positions     .push( Vec.of( unpacked.verts[ 3*j ], unpacked.verts[ 3*j + 1 ], unpacked.verts[ 3*j + 2 ] ) );        
        this.normals       .push( Vec.of( unpacked.norms[ 3*j ], unpacked.norms[ 3*j + 1 ], unpacked.norms[ 3*j + 2 ] ) );
        this.texture_coords.push( Vec.of( unpacked.textures[ 2*j ], unpacked.textures[ 2*j + 1 ]  ));
      }
      this.indices = unpacked.indices;

      this.normalize_positions( false );
      this.copy_onto_graphics_card( this.gl );
      this.ready = true;
    }

  draw( graphics_state, model_transform, material )       // Cancel all attempts to draw the shape before it loads.
    { if( this.ready ) super.draw( graphics_state, model_transform, material );   }
}

class Fake_Bump_Map extends Phong_Shader                        //A Fake Bump Mapping class
{ fragment_glsl_code()           
    { return `
        uniform sampler2D texture;
        void main()
        { if( GOURAUD || COLOR_NORMALS )    // Do smooth "Phong" shading unless options like "Gouraud mode" are wanted instead.
          { gl_FragColor = VERTEX_COLOR;    // Otherwise, we already have final colors to smear (interpolate) across vertices.            
            return;
          }                                 // If we get this far, calculate Smooth "Phong" Shading as opposed to Gouraud Shading.
                                            // Phong shading is not to be confused with the Phong Reflection Model.
          
          vec4 tex_color = texture2D( texture, f_tex_coord );                    // Use texturing as well.
          vec3 bumped_N  = normalize( N + tex_color.rgb - .5*vec3(1,1,1) );      // Slightly disturb normals based on sampling
                                                                                 // the same image that was used for texturing.
                                                                                 
                                                                                 // Compute an initial (ambient) color:
          if( USE_TEXTURE ) gl_FragColor = vec4( ( tex_color.xyz + shapeColor.xyz ) * ambient, shapeColor.w * tex_color.w ); 
          else gl_FragColor = vec4( shapeColor.xyz * ambient, shapeColor.w );
          gl_FragColor.xyz += phong_model_lights( bumped_N );                    // Compute the final color with contributions from lights.
        }`;
    }
}

class Texture_Rotate extends Phong_Shader
{ fragment_glsl_code()            
    {
      return `
        #define PI 3.14159265359
        uniform sampler2D texture;
        void main()
        { if( GOURAUD || COLOR_NORMALS )    // Do smooth "Phong" shading unless options like "Gouraud mode" are wanted instead.
          { gl_FragColor = VERTEX_COLOR;    // Otherwise, we already have final colors to smear (interpolate) across vertices.            
            return;
          }                                 // If we get this far, calculate Smooth "Phong" Shading as opposed to Gouraud Shading.
                                            // Phong shading is not to be confused with the Phong Reflection Model.
          float a = animation_time;
          
          vec4 trans1Col1 = vec4(1,0,0,0);
          vec4 trans1Col2 = vec4(0,1,0,0);
          vec4 trans1Col3 = vec4(0,0,1,0);
          vec4 trans1Col4 = vec4(-0.5,-0.5,0,1);
          mat4 trans1 = mat4(trans1Col1, trans1Col2, trans1Col3, trans1Col4);
          vec4 column1 = vec4(cos(a*PI/1.0),sin(a*PI/1.0),0,0);
          vec4 column2 = vec4(-1.0*sin(a*PI/1.0), cos(a*PI/1.0), 0, 0);
          vec4 column3 = vec4(0,0,1,0);
          vec4 column4 = vec4(0,0,0,1);
          mat4 rotationMat = mat4(column1, column2, column3, column4);
          vec4 trans2Col1 = vec4(1,0,0,0);
          vec4 trans2Col2 = vec4(0,1,0,0);
          vec4 trans2Col3 = vec4(0,0,1,0);
          vec4 trans2Col4 = vec4(0.5,0.5,0,1);
          mat4 trans2 = mat4(trans2Col1, trans2Col2, trans2Col3, trans2Col4);
          vec4 temp_four = vec4(f_tex_coord, 1,1);
          vec4 mult = trans1*temp_four;
          mult = rotationMat*mult;
          mult = trans2*mult;
          vec2 new_tex_coord = mult.xy;
          vec4 tex_color = texture2D( texture, new_tex_coord );                          // Sample the texture image in the correct place.
                                                                                      // Compute an initial (ambient) color:
          if( USE_TEXTURE ) gl_FragColor = vec4( ( tex_color.xyz + shapeColor.xyz ) * ambient, shapeColor.w * tex_color.w ); 
          else gl_FragColor = vec4( shapeColor.xyz * ambient, shapeColor.w );
          gl_FragColor.xyz += phong_model_lights( N );                     // Compute the final color with contributions from lights.
        }`;
    }
}

window.Bollard = window.classes.Tree =
class Bollard extends Shape {
  constructor()
  { super( "positions", "normals", "texture_coords");
    let tree_transform = Mat4.identity();
    tree_transform = tree_transform.times(Mat4.rotation(Math.PI/2, Vec.of(1,0,0))).times(Mat4.translation([-7.5, -15.5, 2.5]))
    this.drawBollard(tree_transform, [[0.25, 0.25], [0.25,0.25]])
    tree_transform = tree_transform.times( Mat4.translation([15, 0, 0]))
    this.drawBollard(tree_transform, [[0.25, 0.25], [0.25,0.25]])
    tree_transform = tree_transform.times( Mat4.translation([0, 31, 0]))
    this.drawBollard(tree_transform, [[0.25, 0.25], [0.25,0.25]])
    tree_transform = tree_transform.times( Mat4.translation([-15, 0, 0]))
    this.drawBollard(tree_transform, [[0.25, 0.25], [0.25,0.25]])
  }
  drawBollard(matrix, texture_coords) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12, texture_coords], matrix.times( Mat4.scale([ 0.3, 0.3, 5]) ))
  }
}



window.Boundary = window.classes.Boundary =
class Boundary extends Shape{
  constructor()
  { super( "positions", "normals", "texture_coords");
    //length of the longer pole is 14
    let boundary_position = Mat4.identity()
    boundary_position = boundary_position.times( Mat4.rotation( Math.PI/2, Vec.of(0,1,0)))
    this.drawLine(boundary_position,  [[0.25, 0.25], [0.25,0.25]])
    boundary_position = boundary_position.times( Mat4.translation( [15, 0, 0]))
    this.drawLine(boundary_position,  [[0.25, 0.25], [0.25,0.25]])
    boundary_position = boundary_position.times( Mat4.translation( [-30, 0, 0]))
    this.drawLine(boundary_position,  [[0.25, 0.25], [0.25,0.25]])
    boundary_position = Mat4.identity(); //reset
    boundary_position = boundary_position.times( Mat4.rotation( Math.PI/2, Vec.of(0,0,1))).times( Mat4.translation([0, -7, 0]))
    this.drawLongerLine(boundary_position,  [[0.25, 0.25], [0.25,0.25]])
    boundary_position = boundary_position.times( Mat4.translation( [0, 14, 0]))
    this.drawLongerLine(boundary_position,  [[0.25, 0.25], [0.25,0.25]])
  }
  drawLine(matrix, texture_coords) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12, texture_coords], matrix.times( Mat4.scale([ 0.2, 0.02, 14]) ))
  }
  drawLongerLine(matrix, texture_coords) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12, texture_coords], matrix.times( Mat4.scale([ 0.02, 0.2, 30]) ))
  }
}

window.Net = window.classes.Net = 
class Net extends Shape
{ constructor()
    { super( "positions", "normals", "texture_coords");
      let field_transform = Mat4.identity()
      //this.drawField(field_transform)
      let h_model_transform = Mat4.identity()
      h_model_transform = h_model_transform.times( Mat4.rotation( Math.PI/2, Vec.of(0,1,0)) )
      this.drawLongerPole(h_model_transform, [[0.25, 0.25], [0.25,0.25]])
      for(let i = 0; i < 8; i++) {
        this.drawHorizontalShapes( h_model_transform, [[0.25,0.25], [0.25,0.25]]) 
        h_model_transform = h_model_transform.times( Mat4.translation([0,-0.25,0]))       
      }
      this.drawLongerPole(h_model_transform, [[0.25, 0.25], [0.25,0.25]])

      let v_model_transform = Mat4.identity()
      v_model_transform = v_model_transform.times( Mat4.translation([7, -2, 0]))
      v_model_transform = v_model_transform.times( Mat4.rotation (Math.PI/2, Vec.of(1,0,0)))
      this.drawVerticalPoles(v_model_transform, [[0.25,0.25], [0.25,0.25]])
      v_model_transform = v_model_transform.times( Mat4.translation([-2.5, 0, -1]))
      for(let i = 0; i < 19; i++) {
        this.drawVerticalShapes( v_model_transform, [[0.25,0.25], [0.25,0.25]])
        v_model_transform = v_model_transform.times( Mat4.translation([-0.5, 0, 0]))
      }
      v_model_transform = v_model_transform.times( Mat4.translation([-2, 0, 1]))
      this.drawVerticalPoles( v_model_transform, [[0.25, 0.25], [0.25, 0.25]])
      //this.drawShapes( Mat4.rotation( -Math.PI/2, Vec.of(1,0,0)).times( Mat4.scale([  1, -1, 1 ])), [[ .25,0.4 ], [ 0,1 ]] ); //default s,t
    }

  //do collision check with Net
  drawHorizontalShapes( matrix, texture_coords ) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12 ,texture_coords], matrix.times( Mat4.scale([ 0.02, 0.02, 9 ]) ) )
  }

  drawLongerPole( matrix, texture_coords ) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12, texture_coords], matrix.times( Mat4.scale([ 0.02, 0.02, 14]) ))
  }

  drawVerticalShapes( matrix, texture_coords ) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12 ,texture_coords], matrix.times( Mat4.scale([ 0.02, 0.02, 2 ]) ) )
  }

  drawVerticalPoles( matrix, texture_coords ) {
    Capped_Cylinder.insert_transformed_copy_into(this, [4, 12, texture_coords], matrix.times( Mat4.scale([ -0.1, 0.3, 6])) )
  }

  check_collision(ball_position) {
    if(Math.abs(ball_position[2][3]) <= 0.02 && ball_position[2][3] >= -0.02 && ball_position[1][3] < 0) { //give 0.05 leeway on top of the net + we need height
      return true;
    }
    return false;
  }
}
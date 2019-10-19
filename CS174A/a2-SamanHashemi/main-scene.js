window.Assignment_Two_Test = window.classes.Assignment_Two_Test =
class Assignment_Two_Test extends Scene_Component
  { constructor( context, control_box )     // The scene begins by requesting the camera, shapes, and materials it will need.
      { super(   context, control_box );    // First, include a secondary Scene that provides movement controls:
        if( !context.globals.has_controls   ) 
          context.register_scene_component( new Movement_Controls( context, control_box.parentElement.insertCell() ) ); 

        context.globals.graphics_state.camera_transform = Mat4.look_at( Vec.of( 0,10,20 ), Vec.of( 0,0,0 ), Vec.of( 0,1,0 ) );
        this.initial_camera_location = Mat4.inverse( context.globals.graphics_state.camera_transform );

        const r = context.width/context.height;
        context.globals.graphics_state.projection_transform = Mat4.perspective( Math.PI/4, r, .1, 1000 );

        const shapes = { torus:  new Torus( 15, 15 ),
                         torus2: new ( Torus.prototype.make_flat_shaded_version() )( 15, 15 ),
                         sphere1: new (Subdivision_Sphere.prototype.make_flat_shaded_version())(1),
                         sphere2: new (Subdivision_Sphere.prototype.make_flat_shaded_version())(2),
                         sphere3: new Subdivision_Sphere(3),
                         sphere4: new Subdivision_Sphere(4)
                          // TODO:  Fill in as many additional shape instances as needed in this key/value table.
                          //        (Requirement 1)
                       }
        this.submit_shapes( context, shapes );
                                     
                                     // Make some Material objects available to you:
        this.materials =
          { test:     context.get_instance( Phong_Shader ).material( Color.of( 1,1,0,1 ), { ambient:.2 } ),
            ring:     context.get_instance( Ring_Shader  ).material(),
            ringboi:  context.get_instance( Phong_Shader  ).material( Color.of(0.445,0.299,0.114,1), {ambient : 0}),
            sun:      context.get_instance( Phong_Shader ).material( Color.of(1,1,0,1), { ambient:1 })
                                // TODO:  Fill in as many additional material objects as needed in this key/value table.
                                //        (Requirement 1)
          }

        this.lights = [ new Light( Vec.of( 5,-10,5,1 ), Color.of( 0, 1, 1, 1 ), 1000 ) ];
      }
    make_control_panel()            // Draw the scene's buttons, setup their actions and keyboard shortcuts, and monitor live measurements.
      { this.key_triggered_button( "View solar system",  [ "0" ], () => this.attached = () => this.initial_camera_location );
        this.new_line();
        this.key_triggered_button( "Attach to planet 1", [ "1" ], () => this.attached = () => this.planet_1 );
        this.key_triggered_button( "Attach to planet 2", [ "2" ], () => this.attached = () => this.planet_2 ); this.new_line();
        this.key_triggered_button( "Attach to planet 3", [ "3" ], () => this.attached = () => this.planet_3 );
        this.key_triggered_button( "Attach to planet 4", [ "4" ], () => this.attached = () => this.planet_4 ); this.new_line();
        this.key_triggered_button( "Attach to planet 5", [ "5" ], () => this.attached = () => this.planet_5 );
        this.key_triggered_button( "Attach to moon",     [ "m" ], () => this.attached = () => this.moon     );
      }
    display( graphics_state )
      { graphics_state.lights = this.lights;        // Use the lights stored in this.lights.
        const t = graphics_state.animation_time / 1000, dt = graphics_state.animation_delta_time / 1000;
        let model_transform = Mat4.identity();
        

        // TODO:  Fill in matrix operations and drawing code to draw the solar system scene (Requirements 2 and 3)
        graphics_state.lights = [new Light (Vec.of(0,0,0,1), Color.of(1 + Math.sin(Math.PI*0.4*t), 0, 1 + Math.sin(Math.PI + Math.PI*0.4*t), 1), 10**(2 + Math.sin(Math.PI*0.4*t)))]; 
        model_transform = model_transform.times( Mat4.scale([2 + Math.sin(Math.PI*0.4*t),2 + Math.sin(Math.PI*0.4*t),2 + Math.sin(Math.PI*0.4*t)] ));
        this.shapes.sphere4.draw(graphics_state, model_transform, this.materials.sun.override( { color: Color.of(1 + Math.sin(Math.PI*0.4*t), 0, 1 + Math.sin(Math.PI + Math.PI*0.4*t), 1)} ));
        
         // Planet 1
        let planet_1 = Mat4.identity();
        planet_1 = planet_1.times(Mat4.rotation(t, Vec.of(0,1,0))).times(Mat4.translation([5,0,0]));
        this.shapes.sphere2.draw(graphics_state, planet_1, this.materials.sun.override( { color: Color.of(0.555,0.642,0.675, 1), ambient: 0} ));
        this.planet_1 = planet_1;

         // Planet 2
        let planet_2 = Mat4.identity();
        planet_2 = planet_2.times(Mat4.rotation(t*0.75, Vec.of(0,1,0))).times(Mat4.translation([8,0,0]));
        this.shapes.sphere3.draw(graphics_state, planet_2, this.materials.sun.override( { color: Color.of(0.183,0.325,0.199, 1), ambient: 0, gouraud: t%2 } ));
        this.planet_2 = planet_2
  
        // Planet 3
        let planet_3 = Mat4.identity();
        planet_3 = planet_3.times(Mat4.rotation(t*0.5, Vec.of(0,1,0))).times(Mat4.translation([11,0,0]));
        this.shapes.sphere4.draw(graphics_state, planet_3, this.materials.sun.override( { color: Color.of(0.445,0.299,0.114,1), ambient: 0} ));
        this.planet_3 = planet_3;

        // Planet 3 Ring
        let planet_4 = planet_3;
        planet_4 = planet_4.times(Mat4.rotation(t*0.5, Vec.of(1, 0, 1))).times(Mat4.translation([0,0,0]));
        planet_4 = planet_4.times(Mat4.scale(Vec.of(1,1,.1)));
        this.shapes.torus.draw(graphics_state, planet_4, this.materials.ringboi);

         // Planet 4
        let planet_5 = Mat4.identity();
        planet_5 = planet_5.times(Mat4.rotation(t*0.25, Vec.of(0,1,0))).times(Mat4.translation([14,0,0]));
        this.shapes.sphere4.draw(graphics_state, planet_5, this.materials.sun.override( { color: Color.of(0.238,0.658,0.815, 1), ambient: 0} ));
        this.planet_4 = planet_5
 
        // Moon
        let moon = planet_5;
        moon = moon.times(Mat4.rotation(t*0.5, Vec.of(0,1,0))).times(Mat4.translation([2,0,0]));
        this.shapes.sphere1.draw(graphics_state, moon, this.materials.sun.override( { color: Color.of(0.238,0.658,0.815, 1), ambient: .2} ));
        this.moon = moon;


        if(this.attached != undefined){
          let desired = this.attached();
          desired = desired.times(Mat4.translation(Vec.of(0,0,5)));
          desired = Mat4.inverse(desired);
          graphics_state.camera_transform = desired.map( (x,i) => Vec.from( graphics_state.camera_transform[i] ).mix( x, 0.1 ) );
        }
      }
  }


// Extra credit begins here (See TODO comments below):

window.Ring_Shader = window.classes.Ring_Shader =
class Ring_Shader extends Shader              // Subclasses of Shader each store and manage a complete GPU program.
{ material() { return { shader: this } }      // Materials here are minimal, without any settings.
  map_attribute_name_to_buffer_name( name )       // The shader will pull single entries out of the vertex arrays, by their data fields'
    {                                             // names.  Map those names onto the arrays we'll pull them from.  This determines
                                                  // which kinds of Shapes this Shader is compatible with.  Thanks to this function, 
                                                  // Vertex buffers in the GPU can get their pointers matched up with pointers to 
                                                  // attribute names in the GPU.  Shapes and Shaders can still be compatible even
                                                  // if some vertex data feilds are unused. 
      return { object_space_pos: "positions" }[ name ];      // Use a simple lookup table.
    }
    // Define how to synchronize our JavaScript's variables to the GPU's:
  update_GPU( g_state, model_transform, material, gpu = this.g_addrs, gl = this.gl )
      { const proj_camera = g_state.projection_transform.times( g_state.camera_transform );
                                                                                        // Send our matrices to the shader programs:
        gl.uniformMatrix4fv( gpu.model_transform_loc,             false, Mat.flatten_2D_to_1D( model_transform.transposed() ) );
        gl.uniformMatrix4fv( gpu.projection_camera_transform_loc, false, Mat.flatten_2D_to_1D(     proj_camera.transposed() ) );
      }
  shared_glsl_code()            // ********* SHARED CODE, INCLUDED IN BOTH SHADERS *********
    { return `precision mediump float;
              varying vec4 position;
              varying vec4 center;
      `;
    }
  vertex_glsl_code()           // ********* VERTEX SHADER *********
    { return `
        attribute vec3 object_space_pos;
        uniform mat4 model_transform;
        uniform mat4 projection_camera_transform;

        void main()
        { 
        }`;           // TODO:  Complete the main function of the vertex shader (Extra Credit Part II).
    }
  fragment_glsl_code()           // ********* FRAGMENT SHADER *********
    { return `
        void main()
        { 
        }`;           // TODO:  Complete the main function of the fragment shader (Extra Credit Part II).
    }
}

window.Grid_Sphere = window.classes.Grid_Sphere =
class Grid_Sphere extends Shape           // With lattitude / longitude divisions; this means singularities are at 
  { constructor( rows, columns, texture_range )             // the mesh's top and bottom.  Subdivision_Sphere is a better alternative.
      { super( "positions", "normals", "texture_coords" );
        

                      // TODO:  Complete the specification of a sphere with lattitude and longitude lines
                      //        (Extra Credit Part III)
      } }
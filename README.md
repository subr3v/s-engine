## S-Engine
Official repository for S-Engine.

Engine programming is my passion, and in order to study and increase my skills in this field I decided to write my own 3D engine from scratch.

This has given me a lot of knowledge on how things work at a very low level: I've tried to write my own implementation for most of the stuff (excluding fbx model loading, lua and zip archives).

Here's an in-depth list of all the features of the engine:

* Core
  * Serialization
  * Endian independent
  * Archive independent (works with memory, files and zipped archives)
  * Runtime Type Information
  * Code profiling and memory tracking facilities
  * Custom memory allocator support
  * Custom Vector class
  * Configuration System
  * Automatic save and loading of configuration at app start-up/shutdown
  * Automatic In-Engine Editor that allows you to modify all the values at runtime
  * Archive system to handle different data sources seamlessly
  * File Archive to handle file loading/saving
  * Memory archive to handle memory manipulation
  * Zipped Archive to handle zipped files archive using zlib
 
* Graphics
  * DirectX 11 Support
  * Render Targets and MRT
  * Support for all types of Shaders (Pixel, Vertex, Geometry, Hull, Domain and Compute)
 
* Rendering
  * Screen Space Ambient Occlusion
  * Forward+ rendering pipeline
  * High Dynamic Range Rendering
  * Physically Based Rendering Shading Model
  * Shadow Mapping
 
* Networking
  * Asynchronous Connection support through TCP/IP protocol
  * Support for both IPv4 and IPv6
  * Packet manipulation utilities to handle endian independent connection and easier serialization of data
 
* Framework
  * Entity Component System architecture
  * Built-in Rendering and Camera System
  * Resource Management
  * Basic Lua Scene loader
  * ImGui library integration within the engine for quick UI tools
  * Basic Lua integration to run custom scripting logic
 
* Asset Management
  * Custom file format for Textures and 3D Models
  * Asset Conversion command line tool
  * Extensible with further dynamic modules loaded at runtime
  * Creates Package files to be loaded from the engine
  * Built-in converters for .fbx and .obj models and most common image formats for textures
 
# Build Instructions
The project uses premake to generate build projects for your target platform (Visual Studio 2012 to 2015), the project has only been tested with Visual Studio 2013 so don't hesitate to contact me if you run into troubles.
In order to compile the engine you need to:
* Go to the Build folder and double click on make_vs_20xx.bat (depending on which version of visual studio you have installed in your machine).
* Go to the generated folder ProjectFiles and open the file Engine_Solution.sln
* Build the project from visual studio!

# Clone Instructions
Please bear in mind that the repo makes use of both git LFS and git submodules, in order to clone the entire repo you will need to type int '''git clone --recursive (path to the repo)''', or alternatively type in '''git submodule update --init --recursive''' after cloning the project to pull all the submodules.

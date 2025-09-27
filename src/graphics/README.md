# Graphics

Build shaders

```sh
cd graphics/neurons/shaders
shaderc.exe -i ../../thirdparty/bgfx/src -f fs_instancing.sc --varyingdef varying_instancing.def.sc -o fs_instancing.bin --type fragment --platform windows --profile s_5_0 -O 3
shaderc.exe -i ../../thirdparty/bgfx/src -f vs_instancing.sc --varyingdef varying_instancing.def.sc -o vs_instancing.bin --type vertex --platform windows --profile s_5_0 -O 3

shaderc.exe -i ../../thirdparty/bgfx/src -f fs_cubes.sc -o fs_cubes.bin --varyingdef varying_cubes.def.sc --type fragment --platform windows --profile s_5_0 -O 3
shaderc.exe -i ../../thirdparty/bgfx/src -f vs_cubes.sc -o vs_cubes.bin --varyingdef varying_cubes.def.sc --type vertex --platform windows --profile s_5_0 -O 3

shaderc.exe -i ../../thirdparty/bgfx/src -f fs_just_color.sc -o vs_bezier_tube.bin --varyingdef varying_bezier_tube.def.sc --type fragment --platform windows --profile s_5_0 -O 3
shaderc.exe -i ../../thirdparty/bgfx/src -f vs_bezier_tube.sc --varyingdef varying_bezier_tube.def.sc -o vs_bezier_tube.bin --type vertex --platform windows --profile s_5_0 -O 3

shaderc.exe -i ../../thirdparty/bgfx/src -f fs_just_color.sc -o vs_tube_test.bin --varyingdef varying_tube_test.def.sc --type fragment --platform windows --profile s_5_0 -O 3
shaderc.exe -i ../../thirdparty/bgfx/src -f vs_tube_test.sc --varyingdef varying_tube_test.def.sc -o vs_tube_test.bin --type vertex --platform windows --profile s_5_0 -O 3
```


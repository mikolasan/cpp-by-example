# Graphics

Build shaders

```sh
cd graphics/neurons/shaders
shaderc.exe -i ../../thirdparty/bgfx/src -f fs_instancing.sc --varyingdef varying_instancing.def.sc -o fs_instancing.bin --type fragment --platform windows --profile s_5_0 -O 3
shaderc.exe -i ../../thirdparty/bgfx/src -f vs_instancing.sc --varyingdef varying_instancing.def.sc -o vs_instancing.bin --type vertex --platform windows --profile s_5_0 -O 3

shaderc.exe -i ../../thirdparty/bgfx/src -f fs_cubes.sc -o fs_cubes.bin --varyingdef varying_cubes.def.sc --type fragment --platform windows --profile s_5_0 -O 3
shaderc.exe -i ../../thirdparty/bgfx/src -f vs_cubes.sc -o vs_cubes.bin --varyingdef varying_cubes.def.sc --type vertex --platform windows --profile s_5_0 -O 3
```

Serialization with Cap'n Proto

```shell
cd graphics\neurons
capnp compile -I"..\thirdparty\capnproto-c++-1.2.0\src" -oc++ neu
ron.capnp
```
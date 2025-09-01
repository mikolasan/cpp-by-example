@0xbd87bc7cc2cac505;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("capgen");

using Location = UInt64;

struct Neuron {
  idx @0 :UInt32;
  v @1 :Float32;
  threshold @2 :Float32;
  spiked @3 :Bool;
}

struct Synapse {
  weight @0 :Float32;
  tracePre @1 :Float32;
  tracePost @2 :Float32;
}

struct SynapseEntry {
  key @0 :Location;
  value @1 :Synapse;
}

struct NeuronLayer {
  neuronIndices @0 :List(UInt32);
}

struct Network {
  dt @0 :Float32;
  time @1 :Float32;

  neurons @2 :List(Neuron);
  layers  @3 :List(NeuronLayer);
  synapses @4 :List(SynapseEntry);
}
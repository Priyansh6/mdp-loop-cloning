add_llvm_component_library(LLVMLoopClone
	LoopClone.cpp
)
add_dependencies(LLVMLoopClone intrinsics_gen)
add_llvm_pass_plugin(LoopClonePass LoopClone.cpp)

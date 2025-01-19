add_llvm_component_library( LLVMAliasHints
		AliasHints.cpp

)
add_dependencies(LLVMAliasHints intrinsics_gen)
add_llvm_pass_plugin(AliasHintsPass AliasHints.cpp)

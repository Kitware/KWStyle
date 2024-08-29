
#if defined(__clang__)


#pragma clang diagnostic push
#if __has_warning("-Wsuggest-override")
	#pragma clang diagnostic ignored "-Wsuggest-override"
#endif

#if __has_warning("-Wsuggest-destructor-override")
	#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif

#if __has_warning("-Wzero-as-null-pointer-constant")
	#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#if __has_warning("-Wredundant-parens")
	#pragma clang diagnostic ignored "-Wredundant-parens"
#endif

#if __has_warning("-Wcomma")
	#pragma clang diagnostic ignored "-Wcomma"
#endif

#if __has_warning("-Wdocumentation")
	#pragma clang diagnostic ignored "-Wdocumentation"
#endif

#if __has_warning("-Wdocumentation-unknown-command")
	#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif

#if __has_warning("-Wcast-qual")
	#pragma clang diagnostic ignored "-Wcast-qual"
#endif

#if __has_warning("-Wswitch-enum")
	#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#if __has_warning("-Wdisabled-macro-expansion")
	#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

#if __has_warning("-Wdeprecated-dynamic-exception-spec")
	#pragma clang diagnostic ignored "-Wdeprecated-dynamic-exception-spec"
#endif

#if __has_warning("-Wdeprecated-copy-with-user-provided-copy")
	#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#endif

#if __has_warning("-Wunused-template")
	#pragma clang diagnostic ignored "-Wunused-template"
#endif

#if __has_warning("-Wsign-conversion")
	#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

#if __has_warning("-Wdeprecated-dynamic-exception-spec")
	#pragma clang diagnostic ignored "-Wdeprecated-dynamic-exception-spec"
#endif

#if __has_warning("-Wshadow")
	#pragma clang diagnostic ignored "-Wshadow"
#endif

#if __has_warning("-Wundef")
	#pragma clang diagnostic ignored "-Wundef"
#endif

#if __has_warning("-Wdeprecated-copy-with-user-provided-dtor")
	#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#endif


#endif
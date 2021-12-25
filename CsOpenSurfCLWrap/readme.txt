Bekannte Probleme des C# Wrappers:
---------------------------------------------------------------------------------
System.TypeInitializationException beim ersten Zugriff auf eine Wrapperfunktion:

	C# Projekte sind normalerweise auf

		<PlatformTarget>AnyCPU</PlatformTarget>

	in der XML Projektdatei gesetzt. Da der Wrapper für einen bestimmten CPU compiliert werden muss,
	sollte die Einstellung beispielsweise auf:

		<PlatformTarget>x86</PlatformTarget>

	gesetzt werden.

---------------------------------------------------------------------------------
Absturz beim Zugriff auf den Konstruktor von CSurfCL

	Im Konstruktor wird die Funktion "clBuildProgram" Verwendet.
	Sie schlägt aus unbekannten Gründen fehl, wenn das C#-Projekt
	auf Windows-Anwendung gestellt ist. Das Projekt auf 
	Konsolenanwendung umstellen hilft vorerst. 
	( Windows Forms usw. können auch dort verwendet werden)



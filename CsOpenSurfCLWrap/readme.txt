Bekannte Probleme des C# Wrappers:
---------------------------------------------------------------------------------
System.TypeInitializationException beim ersten Zugriff auf eine Wrapperfunktion:

	C# Projekte sind normalerweise auf

		<PlatformTarget>AnyCPU</PlatformTarget>

	in der XML Projektdatei gesetzt. Da der Wrapper f�r einen bestimmten CPU compiliert werden muss,
	sollte die Einstellung beispielsweise auf:

		<PlatformTarget>x86</PlatformTarget>

	gesetzt werden.

---------------------------------------------------------------------------------
Absturz beim Zugriff auf den Konstruktor von CSurfCL

	Im Konstruktor wird die Funktion "clBuildProgram" Verwendet.
	Sie schl�gt aus unbekannten Gr�nden fehl, wenn das C#-Projekt
	auf Windows-Anwendung gestellt ist. Das Projekt auf 
	Konsolenanwendung umstellen hilft vorerst. 
	( Windows Forms usw. k�nnen auch dort verwendet werden)



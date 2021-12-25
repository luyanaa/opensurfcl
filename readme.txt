====================================================================================
Hauptordner der c++ Version:

opensurfcl:
	
	* soll den gesamten Code des Projektes enthalten
	* beim erstellen des entsprechenden MakeTarget wird die CsOpenSurfCL.lib erstellt

	
demo:
	
	* enth�lt die C++ Demoversion. Aktuell in erster Linie um den eigenen Code zu testen ;)
	* erstellt eine Demo.exe und bindet die CsOpenSurfCL.lib von opensurfcl ein

test:

	* f�r Unittests. Ist noch nicht richtig eingerichtet. Einfach ignorieren.
====================================================================================

Zu den Wrappern:
BISHER NUR UNTER WINDOWS GETESTET!!!!! 

Ordner f�r Wrappercode: javawrap / cshapwrap
Ordner f�r Wrappercode in der Zielsprache: JOpenSurfCLWrap / CsOpenSurfCLWrap (nicht per cmake verwaltet!!!!!)

* Die beiden Ordner die mit wrap enden enthalten i-Files. 
* Die i-Files sind SWIG-Spezifisch und beschreiben die Erstellung von Wrappern.
* ihr solltet SWIG Version 2.0 installiert haben, wenn ihr die Wrapper selbst erstellen wollt
* cmake ignoriert die beiden Wrapper wenn swig nicht installiert ist

zum JavaWrapper:

javawrap:
	* Erstellt JOpenSurfCL.dll oder JOpenSurfCL.a im build Ordner ( JNI Format )
	* Generiert automatisch .java Sourcefiles! Sie werden im Ordner trunk/JOpenSurfCLWrap/src erstellt
	* Kopiert die JOpenSurfCL.dll in das JOpenSurfCLWrap Verzeichnis

JOpenSurfCLWrap:
	* ist ein Java Netbeans Projekt! Es kann daher nicht von CMake verwaltet werden!
	* Ben�tigt die JOpenSurfCL.dll! Sie wird automatisch durch javawrap in das Projektverzeichnis kopiert
	* Das Projekt compiliert die vom javawrap erstellten .java dateien und Testet sie mit einer Demoapplikation
	* Es wird eine JOpenSurfCLWrap.jar erstellt. Sie ist die Basis f�r die Verwendung in Java. 
	( JOpenSurfCL.dll / JOpenSurfCL.a  wird immer ben�tigt um sie zu verwenden! )
	
csharpwrap:
	* Erstellt die CsOpenSurfCL.dll im build Ordner und Kopiert sie in den trunk\CsOpenSurfCLWrap\CsOpenSurfCLWrap Ordner
	* Generiert automatisch CSharp code im Ordner trunk\CsOpenSurfCLWrap\CsOpenSurfCLWrap
	
CsOpenSurfCLWrap:
	* ist ein VisualStudio Projekt! Geht also nur unter Windows. F�r Linux und Mac m�sste sp�ter ein Monoprojekt erstellt werden.
	* Erstellt die CsOpenSurfCLWrap.dll  mit dem von csharpwrap erstellten csharp code.( die sollte auch mit Mono verwendbar sein )
	* CsOpenSurfCLWrap.dll ist eine CSharp dll! CsOpenSurfCL.dll ist eine c++ dll!
	
	
====================================================================================
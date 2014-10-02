/** @file
Modu� zawiera klasy opakowuj�ce mechanizmy dostarczane przez JVM w klasy C++.
*/ 

#ifndef UNIZETO_JAVACALL_JVMENVIRONMENT_
#define UNIZETO_JAVACALL_JVMENVIRONMENT_

#include <jni.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdexcept>
#include <string>
#include <vector>
#include <map>


/** Namespace zawieraj�cy wszystkie typy wykorzystywane przy wsp�pracy z jav�. */
namespace javacall {

	/** Prywatny namespace zawieraj�cy typy grupuj�ce funkcjonalno�� JVM po typie
	danych, na jakim wykonywana jest operacja.
	Sk�adniki tej przestrzeni nazw przewidziane s� do u�ycia tylko jako pomoc
	w implementacji wrappera dla JNI, i nie powinny by� wykorzystywane w innych
	miejscach programu.
	*/
	namespace _Traits {

		/**
		Szablon, kt�rego konkretne klasy grupuj� wszystkie typy wykorzystywane przy pracy z
		danym typem JNI. Obecnie zawiera on tylko typ tablicy zawieraj�cej elementy danego
		typu JNI.

		Szablon ten mo�e by� u�ywany tylko z parametem typu <code>jobject</code> lub
		pochodnym. Dla ka�dego typu podstawowego musi istnie� jawna specjalizacja.
		Obecnie utworzone s� tylko specjalizacje dla typ�w u�ywanych do tej pory.
		W razie konieczno�ci mo�na utworzy� kolejne, np. dla <code>jfloat</code>,
		<code>jboolean</code>, itd.

		@tparam JniType - <code>jobject</code> lub pochodny.

		@see Types<jbyte>, Types<void>
		*/
		template<typename JniType> class Types {
		
		public:
			/** Typ warto�ci, jakiej oczekuj� funkcje obs�uguj�ce obiekty danego typu JniType.
			Zwykle jest to synonim JniType dla typ�w podstawowych i
			<code>jobject</code> dla typ�w pochodnych po <code>jobject</code>.
			*/
			typedef jobject ValueType;

			/** Typ tablicy zawieraj�cej elementy typu ValueType. Obiekty typu
			<code>jobcject</code> lub pochodnego zawsze s� przechowywane w tablicy
			typu <code>jobjectArray</code>.*/
			typedef jobjectArray ArrayType;
		
		};

		/** Specjalizacja szablonu Types dla typu <code>jbyte</code>.*/
		template<> class Types<jbyte> {
			public:
				typedef jbyte      ValueType;
				typedef jbyteArray ArrayType;
		};

		/** Specjalizacja szablonu Types dla typu <code>void</code>.
		Nie zawiera definicji typu tablicy, poniewa� nie istnieje tablica
		element�w typu <code>void</code>.*/
		template<> class Types<void> {
			public:
				typedef void      ValueType;
		};

		/**
		Szablon klasy, kt�rego konkretyzacje zawieraj� funkcje
		operuj�ce na okre�lonym typie JNI. Konkretyzacja dla typu
		<code>jType</code> grupuje funkcje oferowane dla niego przez interfejs JNI,
		takie jak np. <code>New\<Type>Array</code>, <code>CallStatic\<Type>Method</code>,
		<code>Get\\Set\<Type>ArrayRegion</code>, itd.

		W chwili obejcnej zaimplementowane s� tylko te funkcje, kt�re by�y potrzebne do
		tej pory. W razie potrzeby mo�na zaimplementowa� kolejne.

		@tparam JniType - typ JNI. Konkretyzacja szablonu zawiera funkcje JNI operuj�ce na
		referencjach tego typu.
		*/
		template<typename JniType>
		class FunPtrs {		

			typedef typename Types<JniType>::ArrayType (JNIEnv::*NewArrayPtrType) (jsize len);
			typedef typename Types<JniType>::ValueType (JNIEnv::*CallStaticMethodAPtrType) (jclass classid, jmethodID methodid, const jvalue* args);
			typedef void      (JNIEnv::*SetArrayRegionPtrType)(typename Types<JniType>::ArrayType array, jsize start, jsize len, typename Types<JniType>::ValueType const * buff);
			typedef void      (JNIEnv::*GetArrayRegionPtrType)(typename Types<JniType>::ArrayType array, jsize start, jsize len, typename Types<JniType>::ValueType * buff);

			/** Wska�nik na funkcj� JNI tworz�c� now� tablic� typu JniType. */
			static NewArrayPtrType newArrayPtr;
			/** Wska�nik na funkcj� JNI wywo�uj�c� metod�, kt�ra zwraca warto�� typu JniType. */
			static CallStaticMethodAPtrType callStaticMethodA;
			/** Wska�nik na funkcj� kopiuj�c� zawarto�� tablicy natywnej do tablicy java. */
			static SetArrayRegionPtrType setArrayRegion;
			/** Wska�nik na funkcj� kopiuj�c� zawarto�� tablicy java do tablicy natywnej. */
			static GetArrayRegionPtrType getArrayRegion;		

		public:			

			/** Tworzy now� tablic� java obiekt�w typu JniType.
			@param env - �rodowisko java, w kt�rym ma by� utworzona tablica.
			@param len - wielko�� tablicy

			@return Nowa tablica java element�w typu JniType.
			*/
			static typename Types<JniType>::ArrayType NewArray(JNIEnv* env, jsize len) {
				return (env->*newArrayPtr)(len);
			}

			/** Wywo�uje statyczn� funkcj� java zwracaj�c� warto�� typu JniType.
			@param env - �rodowisko java, w kt�ym ma by� wywo�ana funkcja.
			@param classid - id klasy, do kt�rej nale�y wywo�ywana funkcja.
			@param methodid - id funkcji, kt�ra ma by�wywo�ana.
			@param args  - tablica argument�w, kt�re maj� by� przekazane do funkcji.
				Je�li funkcja nie oczekuje �adnych argument�w, mo�e by� NULL.

			@return Wynik wywo�ania wskazanej funkcji.
			*/
			static JniType CallStaticMethodA(JNIEnv* env, jclass classid, jmethodID methodid, jvalue const * args) {
				return (env->*callStaticMethodA)(classid, methodid, args);
			}


			static void SetArrayRegion(JNIEnv* env, typename Types<JniType>::ArrayType array, jsize start, jsize len, JniType const * buff) {
				(env->*setArrayRegion)(array, start, len, buff);
			}

			static void GetArrayRegion(JNIEnv* env, typename Types<JniType>::ArrayType array, jsize start, jsize len, JniType * buff) {
				(env->*getArrayRegion)(array, start, len, buff);
			}

		};

		/** Specjalizacja szablonu FunPtrs dla typu <code>jobject</code>.
		Specjalizacja konieczna z powodu innego sposobu obs�ugi tablic, ni� w przypadku
		typ�w podstawowych.*/
		template<>
		class FunPtrs<jobject> {

		protected:

			typedef Types<jobject>::ArrayType (JNIEnv::*NewArrayPtrType) (jsize len, jclass clazz, jobject init);
			typedef Types<jobject>::ValueType (JNIEnv::*CallStaticMethodAPtrType) (jclass classid, jmethodID methodid, jvalue const * args);	

			static NewArrayPtrType newArrayPtr;
			static CallStaticMethodAPtrType callStaticMethodA;		

		public:			

			static Types<jobject>::ArrayType NewArray(JNIEnv* env, jsize len, jclass clazz, jobject init) {
				return (env->*newArrayPtr)(len, clazz, init);
			}

			static jobject CallStaticMethodA(JNIEnv* env, jclass classid, jmethodID methodid, jvalue const * args) {
				return (env->*callStaticMethodA)(classid, methodid, args);
			}
		};

		/** Specjalizacja szablonu FunPtrs dla typu <code>void</code>.*/
		template<>
		class FunPtrs<void> {		

		protected:

			typedef Types<void>::ValueType (JNIEnv::*CallStaticMethodAPtrType) (jclass classid, jmethodID methodid, const jvalue* args);				
			static CallStaticMethodAPtrType callStaticMethodA;		

		public:			

			static void CallStaticMethodA(JNIEnv* env, jclass classid, jmethodID methodid, jvalue const * args) {
				(env->*callStaticMethodA)(classid, methodid, args);
			}
		};
	}


	/** Typedef dla mapy string -> string, kt�rej obiekty przechowuj�
	parametry uruchomieniowe JVM.*/
	typedef std::map<std::string, std::string> ParamMap;


	/**
	Klasa stanowi�ca front-end dla funkcjonalno��i �rodowiska JVM, takiej jak �adowanie klas
	java, wywo�ywanie metod, tworzenie obiekt�w, tablic i generalnie ca�a funkcjonalno��
	oferowana przez jvm.dll.

	TODO: Jeszcze nie wszystkie operacje udost�pniane przez JVM s� zawarte w tej klasie,
	na razie znajduje si� tu tylko funkcjinalno�� do tej pory wykorzystywana. W razie
	potrzeby mo�na j� poszerzy�.

	Obiekty tej klasy s� niekopiowalne.

	Ka�da z metod publicznych klasy rzuca wyj�tek JvmException w razie niepowodzenia, chyba,
	�e zaznaczono inaczej.
	*/
	class JvmEnvironment {

		/** Prywatny konstruktor kopiuj�cy. Non - copyable. */
		JvmEnvironment(const JvmEnvironment&);
		/** Prywatny operator przypisania. Non - assignable. */
		JvmEnvironment& operator=(const JvmEnvironment&);

		/** Flaga m�wi�ca, czy ta instancja owija JVM z g��wnego w�tku, czy
		z w�tku pobocznego.*/
		bool attached;

		/** Flaga m�wi�ca, czy ta instancja klasy inicjalizowa�a JVM, i czy tym
		samym mo�e pr�bowa� j� zamkn��.*/
		bool canDestroy;

		/**
		Dokonuje linkowania w czasie wykonania z bibliotek� dynamiczn� <b>jvm.dll</b>.
		
		W chwili obecnej linkowane s� tylko te funkcje, kt�re s� wykorzystywane przez 
		klas�. W razie potrzeby mo�na doda� linkowanie z innymi wymaganymi funkcjami.

		@param dllPath - �cie�ka do pliku dll, kt�ry eksportuje funkcjonalno�� JVM.
		Mo�e by� NULL. W przypaku, gdy linkowanie z plikiem we wskazanej lokalizacji
		si� nie powiedzie, nast�puje pr�ba zlinkowania z plikiem <b>jvm.dll</b> znajduj�cym
		si� w katalogu bie��cym aplikacji.
		*/
		void LoadJvmDll(const char* dllPath);

		/** Uchwyt do modu�u biblioteki dynamicznej JVM. */
#ifdef WIN32		
		HMODULE jvmLib;
#else 
		void *jvmLib;		
#endif		

		typedef jint (JNICALL *PROC_GET_DEFAULT_JVM_ARGS)(void *args);
		typedef jint (JNICALL *PROC_CREATE_VM)(JavaVM **pvm, void **penv, void *args);
		typedef jint (JNICALL *PROC_GET_CREATED_VMS)(JavaVM **, jsize, jsize *);
		
		PROC_CREATE_VM fnCreateJavaVM;
		PROC_GET_CREATED_VMS fnGetCreatedJavaVMs;
		PROC_GET_DEFAULT_JVM_ARGS fnGetDefaultJavaVMInitArgs;


	protected:
		/** Pomocniczy typedef dla obiekt�w reprezentuj�cych tablic� argument�w
		przekazywanych do metod. */
		typedef std::vector<jvalue> ArgsArray;

		/** Uchwyt do JVM zainicjalizowanej przez t� klas�. */
		JavaVM *vm;
		/** Uchwyt do �rodowiska java reprezentowanego przez instancj� tej klasy. */
		JNIEnv *env;

		/** Funkcja pomocnicza, sprawdza, czy w JVM zosta�o zasygnalizowane wyst�pienie
		wyj�tku, a je�li tak, to rzuca wyj�tek JvmException z wiadomo�ci� i stacktrace
		pobranymi z JVM.

		Funkcja ta powinna by� wywo�ana po ka�dej operacji na JVM, aby sprawdzi�, czy
		w JVM nie nast�pi� wyj�tek. W razie potrzeby mo�na zrezygnowa� z wywo�ania tej
		funckji na rzecz specyficznej dla sytuacji obs�ugi b��du, jednak po wykonaniu
		ka�dej operacji na JVM zawsze trzeba sprawdzi�, czy b��d nie nast�pi�, i je�li tak,
		trzeba ten b��d obs�u�y�.

		@throws JvmException.
		*/
		/*virtual*/ void checkException();

	public:

		/**
		Odnajduje w �rodowisku java klas� o podanym deskryptorze.
		@param name - deskryptor klasy, kt�ra ma zostac odnaleziona.

		@returns Referencja do obiektu reprezentuj�cego odnalezion� klas�.
		
		@throws JvmException - nast�pi� wyj�tek java.
		*/
		virtual jclass findClass(const std::string& name);
		
		/**
		Odnajduje w �rodowisku java metod� o podej nazwie i deskryptorze.

		@param clazz - referencja do klasy, w kt�ej ma by� odnaleziona metoda.
		@param name - nazwa metody, kt�ra ma zostac odnaleziona.
		@param signature - deskryptor metody, kt�ra ma zostac odnaleziona.

		@returns Identyfikator odnalezionej metody, kt�ry mo�e by� u�yty do jej wywo�ania.
		
		@throws JvmException - nast�pi� wyj�tek java.
		*/
		virtual jmethodID findMethod(jclass clazz, const std::string& name, const std::string& signature);

		/**
		Konstruktor. Nie inicjalizuje JVM, po utworzeniu obiektu nale�y wywo�a� funkcj�
		sk�adow� init.
		*/
		JvmEnvironment();

		/** Wirtualny destruktor. */
		virtual ~JvmEnvironment();

		/** Zwraca uchwyt do JVM owijanej przez instancj� tej klasy. */
		JavaVM* getVM() const { return vm; }

		/**
		Inicjalizuje JVM z podanymi parametrami. Je�li w ramach procesu nie
		istnieje jeszcze zainicjalizowana VM, zostaje ona utworzona. Je�li VM
		istnieje ju� w ramach tego samego w�tku, obiekt jest inicjalizowany
		ju� istniej�cym �rodowiskiem java. Je�li metoda ta jest wywo�ywana z w�tku
		pobocznego, w kt�rym JVM nie jest jeszcze zainicjalizowana, w�tek jest
		do��czany do istniej�cej VM i tworzone jest w nim nowe �rodowisko java.

		@param params - mapa zawieraj�ca dodatkowe parametry uruchomieniowe maszyny
						wirtualnej. Jako klucze w mapie powinny znajdowa� si� nazwy
						parametr�w (ze znakiem '=', je�li jest konieczny), a jako
						warto�ci - warto�ci parametr�w. Je�li dany parametr nie posiada
						warto�ci (np. jaka� flaga, jak <code>-ea</code>), do mapy powinna
						by� w�o�ona pusta warto��. Przyk�adowo, mapa z parametrami
						uruchomieniowymi mo�e wygl�da� tak:
\code
"-Xmx"                =>  "256m"
"-Djava.class.path="  =>  ".\plik1.jar;.\lib\plik2.jar;"
"-jni:verbatim"       =>  ""
\endcode

		@param libPath - �cie�ka do biblioteki DLL zawieraj�cej JVM. Mo�e by� NULL.

		@see LoadJvmDll.

		@throws std::runtime_error - inicjalizacja JVM nie powiod�a si�.
		*/
		virtual void init(const char* libPath, const ParamMap& params);

		/**
		Wywo�uje statyczn� funcj� java.

		@tparam JniType - typ zwracany wywo�ywanej funkcji statycznej. Na podstawie tego
		    parametru rozstrzygane jest, kt�ra funkcja �rodowiska java jest
			wywo�ywana (CallStaticVoidMethod, CallStaticIntMethod, itp).

		@param classId - id klasy, na rzecz kt�rej ma zosta� wywo�ana sk�adowa funkcja
		statyczna.
		@param methodID - identyfikator metody, kt�ra ma zosta� wywo�ana.
		@param args - argumenty, kt�re maj� zosta� przekazane do funkcji. Domy�lnie
		pusta tablica (brak argument�w).

		@returns Warto�� zwr�cona przez funkcj� java.

		@throws JvmException - w czasie wykonania funkcji statycznej wyst�pi�
		wyj�tek java.
		*/
		template<typename JniType>
		JniType callStaticMethod(jclass classId, jmethodID methodID, const ArgsArray& args = ArgsArray()) {
			return callStaticMethod<JniType>(classId, methodID, args.size() ? &args[0] : NULL);
		}

		/** Wariant funkcji umo�liwiaj�cy zwracanie warto�ci typu innego ni�
		typ, na rzecz kt�rego jest wywo�ywana, bez potrzeby rzutowania
		w kodzie wywo�uj�cym. Najcz�ciej stosowana w przypadku funkcji, kt�re zwracaj�
		referencje typu pochodnego od <code>jobcject</code>. W�wczas przydaje si� ona
		w np. takich sytuacjach:

		\code
		//z konieczno�ci� jawnego rzutowania:
		jbyteArray array = static_cast<jbyteArray>(callStaticMethod<jobject>(someClass, getArrayMethodId));

		//z wykorzystaniem tej wersji funkcji:
		jbyteArray array = callStaticMethod<jobject, jbyteArray>(someClass, getArrayMethodId);
		\endcode

		@see callStaticMethod
		*/
		template<typename JniType, typename CastTo>
		CastTo callStaticMethod(jclass classId, jmethodID methodID, const ArgsArray& args = ArgsArray()) {
			return static_cast<CastTo>(callStaticMethod<JniType>(classId, methodID, args.size() ? &args[0] : NULL));
		}

		/**
		Wywo�uje statyczn� funcj� java.

		@tparam JniType - typ zwracany wywo�ywanej funkcji statycznej. Na podstawie tego
		    parametru rozstrzygane jest, kt�ra funkcja �rodowiska java jest
			wywo�ywana (CallStaticVoidMethod, CallStaticIntMethod, itp).

		@param classId - id klasy, na rzecz kt�rej ma zosta� wywo�ana sk�adowa funkcja
		statyczna.
		@param methodID - identyfikator metody, kt�ra ma zosta� wywo�ana.
		@param args - c-tablica zawieraj�ca argumenty, kt�re maj� zosta� przekazane
		do funkcji. Mo�e by� NULL - oznacza to brak argument�w.

		@returns Warto�� zwr�cona przez funkcj� java.

		@throws JvmException - w czasie wykonania funkcji statycznej wyst�pi�
		wyj�tek java.
		*/
		template<typename JniType>
		JniType callStaticMethod(jclass classId, jmethodID methodID, const jvalue* args) {

			JniType retval;
			retval = _Traits::FunPtrs<JniType>::CallStaticMethodA(env, classId, methodID, args);			

			checkException();

			return retval;
		}

		/** Tworzy now� tablic� java obiekt�w typu prostego.
		@tparam JniType - typ obiekt�w, kt�rych tablica ma zosta� utworzona. 
			Nie mo�e by� <code>jobject</code> ani pochodny, w tym celu nale�y u�y�
			newArray(jsize, jclass).

		@param size - wielko�� tablicy.

		@return Nowa tablica java.

		@throws JvmException - nast�pi� wyj�tek java.
		*/
		template <typename JniType> typename _Traits::Types<JniType>::ArrayType newArray(jsize size) {

			typename _Traits::Types<JniType>::ArrayType arr = _Traits::FunPtrs<JniType>::NewArray(env, size);
			checkException();
			return arr;;
		}

		/** Tworzy tablic� java obiekt�w typu <code>jobject</code> lub pochodnego.
		@param size - wielko�� tablicy.
		@param clazz - id klasy java, kt�rej zawiera tablica.

		@return Nowa tablica obiekt�w typu okre�lonego przez <code>clazz</code>,
			zainicjalizowana warto�ciami <code>null</code>.

		@throws JvmException - nast�pi� wyj�tek java.
		*/
		_Traits::Types<jobject>::ArrayType newArray(jsize size, jclass clazz) {

			_Traits::Types<jobject>::ArrayType arr = _Traits::FunPtrs<jobject>::NewArray(env, size, clazz, NULL);
			checkException();
			return arr;
		}

		/** Konwertuje tablic� natywn� na tablic� java.
		Przydatna przy przesy�aniu argument�w z funkcji natywnej do funkcji java.
		
		Obecnie nie istnieje wersja tej funkcji dla tablic przechowuj�cych obiekty
		typu <code>jobject</code> lub pochodnego - zaleca si� tworzenie interfejsu
		w taki spos�b, aby do funkcji java przesy�a� argumenty jak najprostszych typ�w
		(podstawowych lub ich tablice).

		@tparam - typ JNI obiekt�w zawartych w tablicy. Mo�e by� to tylko typ podstawowy.

		@param nativeArray - tablica natywna do przekonwertowania
		@return Tablica java zawieraj�ca elementy typu JniType. Ma tak� sam� wielko��,
		jak tablica wej�ciowa, i zawiera dok�adnie te same referencje.

		@throws JvmException - nast�pi� wyj�tek java.
		*/
		template<typename JniType>
		typename _Traits::Types<JniType>::ArrayType toJavaArray(const std::vector<JniType>& nativeArray) {
			
			typename _Traits::Types<JniType>::ArrayType resultArray;
			resultArray = newArray<JniType>(nativeArray.size());

			_Traits::FunPtrs<JniType>::SetArrayRegion(env, resultArray, 0, nativeArray.size(), &nativeArray[0]);
			checkException();

			return resultArray;
		}

		/** Konwertuje tablic� java zawieraj�c� obiekty typu prostego na tablic�
		natywn�.
		Funkcja pomocna w aplikacji natywnej przy odbieraniu tablic java
		zwr�conych przez funckje java.
		
		@tparam JniType - typ obiekt�w przechowywanych w tablicy.

		@param javaArray - tablica java do przekonwertowania

		@return Tablica natywna zawieraj�ca te same referencje, kt�re zawarte by�y
			w wej�ciowej tablicy java.

		@throws JvmException - nast�pi� wyj�tek java.
		*/
		template <typename JniType>
		std::vector<JniType> toNativeArray(typename _Traits::Types<JniType>::ArrayType javaArray) {

			std::vector<JniType> nativeArray;

			jsize size = env->GetArrayLength(javaArray);

			if(size) {
				nativeArray.resize(size);
				_Traits::FunPtrs<JniType>::GetArrayRegion(env, javaArray, 0, size, &nativeArray[0]);
				checkException();
			}

			return nativeArray;
		}


		/** Funkcja pomocnicza, kt�ra pozwala na konwersj� tablicy java
		przechowuj�cej obiekty typu pochodnego od <code>jobject</code>
		na tablic� natywn� zawieraj�c� tak�e obiekty typu pochodnego od
		<code>jobject</code>. Dzi�ki temu podczas pracy ze zwr�con� tablic�
		nie jest konieczne rzutowanie jej element�w:

		\code
		//z u�yciem funkcji toNativeArray<jobject>(jobjectArray javaArray):
		jobjectArray strings = getStrings(); //funkcja zwracaj�ca tablic� java string�w
		std::vector<jobject> nativeArray = toNativeArray<jobject>(strings);
		jstring someString = static_cast<jstring>(nativeArray[index]); //konieczne rzutowanie

		//z u�yciem tej funkcji pomocniczej:
		jobjectArray strings = getStrings(); //funkcja zwracaj�ca tablic� java string�w
		std::vector<jstring> nativeArray = toNativeArrayOf<jobject, jstring>(strings);
		jstring someString = nativeArray[index]; //brak rzutowania
		\endcode
		
		@see toNativeArray<jobject>(jobjectArray javaArray)
		*/
		template <typename ResultElemType, typename ArrayType>
		std::vector<ResultElemType> toNativeArrayOf(ArrayType javaArray) {

			std::vector<ResultElemType> nativeArray;

			jsize size = env->GetArrayLength(javaArray);

			if(size) {
				nativeArray.resize(size);
				
				for(int i=0; i<size; ++i) {
					nativeArray[i] = static_cast<ResultElemType>(env->GetObjectArrayElement(javaArray, i));
					checkException();
				}
			}

			return nativeArray;
		}
	};

	/** Specjalizacja dla wywo�ywania funkcji nie zwracaj�cych  warto�ci.
	@see callStaticMethod(jclass, jmethodID, const jvalue*)
	@returns Nie zwraca warto�ci.
	*/
	template<>
	void JvmEnvironment::callStaticMethod<void>(jclass classId, jmethodID methodID, const jvalue* args);


	/** Specjalizacja dla tablic java przechowuj�cych obiekty typu
	<code>jobject</code>.
	
	@see toNativeArray(typename _Traits::Types<JniType>::ArrayType)
	*/
	template <>
	std::vector<jobject> JvmEnvironment::toNativeArray<jobject>(jobjectArray javaArray);


	/** Pomocnicza klasa wspomagaj�ca RAII podczas pracy z lokalnymi referencjami
	zwracanymi przez funkcje JVM. Obiekty klasy staj� si� w�a�cicielami referencji,
	i zwalniaj� j� w chwili wywo�ania destruktora.

	@tparam JniType - typ referencji, kt�ra jest owijana. Domy�lnie <code>jobject</code>.
	*/
	template <typename JniType = jobject>
	class LocalRef {

	private:
		LocalRef(const LocalRef& );
		LocalRef& operator= (const LocalRef& );

		JNIEnv* env_;	
		JniType ref_;

	public:

		/** Udost�pnia uchwyt do przechowywanej referencji.
		@returns Owijana referencja.
		*/
		JniType get() const {
			return ref_;
		}

		/** Konstruktor.
		@param ref - referencja, kt�ra ma zosta� zwolniona. Mo�e by� NULL.
		@param env - �rodowisko java, z kt�rego pochodzi referencja. */
		LocalRef(JniType ref, JNIEnv* env) : env_(env), ref_(ref) {
		}

		/** Destruktor. Zwalnia owijan� referencj�. */
		~LocalRef() {
			env_->DeleteLocalRef(ref_);
		}
	};

	/**
	Klasa reprezentuj�ca og�lny wyj�tek, kt�ry wystapi� podczas wykonywania kodu
	java. W razie potrzeby mo�na wyprowadzi� od tej klasy bardziej szczeg�ow�
	hierarchi� wyj�tk�w.
	*/
	class JvmException : public std::runtime_error {

	protected:
	
		/** Nazwa klasy wyj�tku Java, kt�ry spowodowa� wyst�pienie tego wyj�tku.*/
		std::string className_;

		/** Stacktrace wyj�tku. */
		std::string stackTrace_;

		
		/** Konstruktor tworz�cy obiekt wyj�tku java z podanym komunikatem, oraz
		wype�nionym stacktrace. Powinien by� u�ywany tylko w chwili obs�ugi wyj�tku
		java, kt�ry wyst�pi� w maszynie wirtualnej. */
		JvmException(const std::string& msg, const std::string& className, const std::string& stacktrace) : 
			std::runtime_error(msg),
			className_(className),
			stackTrace_(stacktrace) {}

	public:
		/** Zwraca stacktrace java wyj�tku. W przypadku, gdy w czasie pobierania
		informacji o wyj�tku wyst�pi� kolejny b��d, stacktrace mo�e by� pusty.
		@return Stacktrace wyj�tku. */
		std::string getStackTrace() const {return stackTrace_; }

		/** Zwraca nazw� klasy java wyj�tku. W przypadku, gdy w czasie pobierania
		informacji o wyj�tku wyst�pi� kolejny b��d, nazwa mo�e by� pusta.
		@return Nazwa klasy Java wyj�tku. */
		virtual std::string getClassName() const {return className_; }

		/** Konstruktor. Tworzy wyj�tek JVM z opisem dostarczonym przez u�ytkownika. */
		JvmException(const std::string& msg) : std::runtime_error(msg) {};
		

		~JvmException() throw() {};
		
		friend class javacall::JvmEnvironment;
		
		
	};

} //namespace javacall


#endif //UNIZETO_JAVACALL_JVMENVIRONMENT_

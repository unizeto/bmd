/** @file
Modu³ zawiera klasy opakowuj¹ce mechanizmy dostarczane przez JVM w klasy C++.
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


/** Namespace zawieraj¹cy wszystkie typy wykorzystywane przy wspó³pracy z jav¹. */
namespace javacall {

	/** Prywatny namespace zawieraj¹cy typy grupuj¹ce funkcjonalnoœæ JVM po typie
	danych, na jakim wykonywana jest operacja.
	Sk³adniki tej przestrzeni nazw przewidziane s¹ do u¿ycia tylko jako pomoc
	w implementacji wrappera dla JNI, i nie powinny byæ wykorzystywane w innych
	miejscach programu.
	*/
	namespace _Traits {

		/**
		Szablon, którego konkretne klasy grupuj¹ wszystkie typy wykorzystywane przy pracy z
		danym typem JNI. Obecnie zawiera on tylko typ tablicy zawieraj¹cej elementy danego
		typu JNI.

		Szablon ten mo¿e byæ u¿ywany tylko z parametem typu <code>jobject</code> lub
		pochodnym. Dla ka¿dego typu podstawowego musi istnieæ jawna specjalizacja.
		Obecnie utworzone s¹ tylko specjalizacje dla typów u¿ywanych do tej pory.
		W razie koniecznoœci mo¿na utworzyæ kolejne, np. dla <code>jfloat</code>,
		<code>jboolean</code>, itd.

		@tparam JniType - <code>jobject</code> lub pochodny.

		@see Types<jbyte>, Types<void>
		*/
		template<typename JniType> class Types {
		
		public:
			/** Typ wartoœci, jakiej oczekuj¹ funkcje obs³uguj¹ce obiekty danego typu JniType.
			Zwykle jest to synonim JniType dla typów podstawowych i
			<code>jobject</code> dla typów pochodnych po <code>jobject</code>.
			*/
			typedef jobject ValueType;

			/** Typ tablicy zawieraj¹cej elementy typu ValueType. Obiekty typu
			<code>jobcject</code> lub pochodnego zawsze s¹ przechowywane w tablicy
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
		Nie zawiera definicji typu tablicy, poniewa¿ nie istnieje tablica
		elementów typu <code>void</code>.*/
		template<> class Types<void> {
			public:
				typedef void      ValueType;
		};

		/**
		Szablon klasy, którego konkretyzacje zawieraj¹ funkcje
		operuj¹ce na okreœlonym typie JNI. Konkretyzacja dla typu
		<code>jType</code> grupuje funkcje oferowane dla niego przez interfejs JNI,
		takie jak np. <code>New\<Type>Array</code>, <code>CallStatic\<Type>Method</code>,
		<code>Get\\Set\<Type>ArrayRegion</code>, itd.

		W chwili obejcnej zaimplementowane s¹ tylko te funkcje, które by³y potrzebne do
		tej pory. W razie potrzeby mo¿na zaimplementowaæ kolejne.

		@tparam JniType - typ JNI. Konkretyzacja szablonu zawiera funkcje JNI operuj¹ce na
		referencjach tego typu.
		*/
		template<typename JniType>
		class FunPtrs {		

			typedef typename Types<JniType>::ArrayType (JNIEnv::*NewArrayPtrType) (jsize len);
			typedef typename Types<JniType>::ValueType (JNIEnv::*CallStaticMethodAPtrType) (jclass classid, jmethodID methodid, const jvalue* args);
			typedef void      (JNIEnv::*SetArrayRegionPtrType)(typename Types<JniType>::ArrayType array, jsize start, jsize len, typename Types<JniType>::ValueType const * buff);
			typedef void      (JNIEnv::*GetArrayRegionPtrType)(typename Types<JniType>::ArrayType array, jsize start, jsize len, typename Types<JniType>::ValueType * buff);

			/** WskaŸnik na funkcjê JNI tworz¹c¹ now¹ tablicê typu JniType. */
			static NewArrayPtrType newArrayPtr;
			/** WskaŸnik na funkcjê JNI wywo³uj¹c¹ metodê, która zwraca wartoœæ typu JniType. */
			static CallStaticMethodAPtrType callStaticMethodA;
			/** WskaŸnik na funkcjê kopiuj¹c¹ zawartoœæ tablicy natywnej do tablicy java. */
			static SetArrayRegionPtrType setArrayRegion;
			/** WskaŸnik na funkcjê kopiuj¹c¹ zawartoœæ tablicy java do tablicy natywnej. */
			static GetArrayRegionPtrType getArrayRegion;		

		public:			

			/** Tworzy now¹ tablicê java obiektów typu JniType.
			@param env - œrodowisko java, w którym ma byæ utworzona tablica.
			@param len - wielkoœæ tablicy

			@return Nowa tablica java elementów typu JniType.
			*/
			static typename Types<JniType>::ArrayType NewArray(JNIEnv* env, jsize len) {
				return (env->*newArrayPtr)(len);
			}

			/** Wywo³uje statyczn¹ funkcjê java zwracaj¹c¹ wartoœæ typu JniType.
			@param env - œrodowisko java, w któym ma byæ wywo³ana funkcja.
			@param classid - id klasy, do której nale¿y wywo³ywana funkcja.
			@param methodid - id funkcji, która ma byæwywo³ana.
			@param args  - tablica argumentów, które maj¹ byæ przekazane do funkcji.
				Jeœli funkcja nie oczekuje ¿adnych argumentów, mo¿e byæ NULL.

			@return Wynik wywo³ania wskazanej funkcji.
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
		Specjalizacja konieczna z powodu innego sposobu obs³ugi tablic, ni¿ w przypadku
		typów podstawowych.*/
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


	/** Typedef dla mapy string -> string, której obiekty przechowuj¹
	parametry uruchomieniowe JVM.*/
	typedef std::map<std::string, std::string> ParamMap;


	/**
	Klasa stanowi¹ca front-end dla funkcjonalnoœæi œrodowiska JVM, takiej jak ³adowanie klas
	java, wywo³ywanie metod, tworzenie obiektów, tablic i generalnie ca³a funkcjonalnoœæ
	oferowana przez jvm.dll.

	TODO: Jeszcze nie wszystkie operacje udostêpniane przez JVM s¹ zawarte w tej klasie,
	na razie znajduje siê tu tylko funkcjinalnoœæ do tej pory wykorzystywana. W razie
	potrzeby mo¿na j¹ poszerzyæ.

	Obiekty tej klasy s¹ niekopiowalne.

	Ka¿da z metod publicznych klasy rzuca wyj¹tek JvmException w razie niepowodzenia, chyba,
	¿e zaznaczono inaczej.
	*/
	class JvmEnvironment {

		/** Prywatny konstruktor kopiuj¹cy. Non - copyable. */
		JvmEnvironment(const JvmEnvironment&);
		/** Prywatny operator przypisania. Non - assignable. */
		JvmEnvironment& operator=(const JvmEnvironment&);

		/** Flaga mówi¹ca, czy ta instancja owija JVM z g³ównego w¹tku, czy
		z w¹tku pobocznego.*/
		bool attached;

		/** Flaga mówi¹ca, czy ta instancja klasy inicjalizowa³a JVM, i czy tym
		samym mo¿e próbowaæ j¹ zamkn¹æ.*/
		bool canDestroy;

		/**
		Dokonuje linkowania w czasie wykonania z bibliotek¹ dynamiczn¹ <b>jvm.dll</b>.
		
		W chwili obecnej linkowane s¹ tylko te funkcje, które s¹ wykorzystywane przez 
		klasê. W razie potrzeby mo¿na dodaæ linkowanie z innymi wymaganymi funkcjami.

		@param dllPath - œcie¿ka do pliku dll, który eksportuje funkcjonalnoœæ JVM.
		Mo¿e byæ NULL. W przypaku, gdy linkowanie z plikiem we wskazanej lokalizacji
		siê nie powiedzie, nastêpuje próba zlinkowania z plikiem <b>jvm.dll</b> znajduj¹cym
		siê w katalogu bie¿¹cym aplikacji.
		*/
		void LoadJvmDll(const char* dllPath);

		/** Uchwyt do modu³u biblioteki dynamicznej JVM. */
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
		/** Pomocniczy typedef dla obiektów reprezentuj¹cych tablicê argumentów
		przekazywanych do metod. */
		typedef std::vector<jvalue> ArgsArray;

		/** Uchwyt do JVM zainicjalizowanej przez t¹ klasê. */
		JavaVM *vm;
		/** Uchwyt do œrodowiska java reprezentowanego przez instancjê tej klasy. */
		JNIEnv *env;

		/** Funkcja pomocnicza, sprawdza, czy w JVM zosta³o zasygnalizowane wyst¹pienie
		wyj¹tku, a jeœli tak, to rzuca wyj¹tek JvmException z wiadomoœci¹ i stacktrace
		pobranymi z JVM.

		Funkcja ta powinna byæ wywo³ana po ka¿dej operacji na JVM, aby sprawdziæ, czy
		w JVM nie nast¹pi³ wyj¹tek. W razie potrzeby mo¿na zrezygnowaæ z wywo³ania tej
		funckji na rzecz specyficznej dla sytuacji obs³ugi b³êdu, jednak po wykonaniu
		ka¿dej operacji na JVM zawsze trzeba sprawdziæ, czy b³¹d nie nast¹pi³, i jeœli tak,
		trzeba ten b³¹d obs³u¿yæ.

		@throws JvmException.
		*/
		/*virtual*/ void checkException();

	public:

		/**
		Odnajduje w œrodowisku java klasê o podanym deskryptorze.
		@param name - deskryptor klasy, która ma zostac odnaleziona.

		@returns Referencja do obiektu reprezentuj¹cego odnalezion¹ klasê.
		
		@throws JvmException - nast¹pi³ wyj¹tek java.
		*/
		virtual jclass findClass(const std::string& name);
		
		/**
		Odnajduje w œrodowisku java metodê o podej nazwie i deskryptorze.

		@param clazz - referencja do klasy, w któej ma byæ odnaleziona metoda.
		@param name - nazwa metody, która ma zostac odnaleziona.
		@param signature - deskryptor metody, która ma zostac odnaleziona.

		@returns Identyfikator odnalezionej metody, który mo¿e byæ u¿yty do jej wywo³ania.
		
		@throws JvmException - nast¹pi³ wyj¹tek java.
		*/
		virtual jmethodID findMethod(jclass clazz, const std::string& name, const std::string& signature);

		/**
		Konstruktor. Nie inicjalizuje JVM, po utworzeniu obiektu nale¿y wywo³aæ funkcjê
		sk³adow¹ init.
		*/
		JvmEnvironment();

		/** Wirtualny destruktor. */
		virtual ~JvmEnvironment();

		/** Zwraca uchwyt do JVM owijanej przez instancjê tej klasy. */
		JavaVM* getVM() const { return vm; }

		/**
		Inicjalizuje JVM z podanymi parametrami. Jeœli w ramach procesu nie
		istnieje jeszcze zainicjalizowana VM, zostaje ona utworzona. Jeœli VM
		istnieje ju¿ w ramach tego samego w¹tku, obiekt jest inicjalizowany
		ju¿ istniej¹cym œrodowiskiem java. Jeœli metoda ta jest wywo³ywana z w¹tku
		pobocznego, w którym JVM nie jest jeszcze zainicjalizowana, w¹tek jest
		do³¹czany do istniej¹cej VM i tworzone jest w nim nowe œrodowisko java.

		@param params - mapa zawieraj¹ca dodatkowe parametry uruchomieniowe maszyny
						wirtualnej. Jako klucze w mapie powinny znajdowaæ siê nazwy
						parametrów (ze znakiem '=', jeœli jest konieczny), a jako
						wartoœci - wartoœci parametrów. Jeœli dany parametr nie posiada
						wartoœci (np. jakaœ flaga, jak <code>-ea</code>), do mapy powinna
						byæ w³o¿ona pusta wartoœæ. Przyk³adowo, mapa z parametrami
						uruchomieniowymi mo¿e wygl¹daæ tak:
\code
"-Xmx"                =>  "256m"
"-Djava.class.path="  =>  ".\plik1.jar;.\lib\plik2.jar;"
"-jni:verbatim"       =>  ""
\endcode

		@param libPath - œcie¿ka do biblioteki DLL zawieraj¹cej JVM. Mo¿e byæ NULL.

		@see LoadJvmDll.

		@throws std::runtime_error - inicjalizacja JVM nie powiod³a siê.
		*/
		virtual void init(const char* libPath, const ParamMap& params);

		/**
		Wywo³uje statyczn¹ funcjê java.

		@tparam JniType - typ zwracany wywo³ywanej funkcji statycznej. Na podstawie tego
		    parametru rozstrzygane jest, która funkcja œrodowiska java jest
			wywo³ywana (CallStaticVoidMethod, CallStaticIntMethod, itp).

		@param classId - id klasy, na rzecz której ma zostaæ wywo³ana sk³adowa funkcja
		statyczna.
		@param methodID - identyfikator metody, która ma zostaæ wywo³ana.
		@param args - argumenty, które maj¹ zostaæ przekazane do funkcji. Domyœlnie
		pusta tablica (brak argumentów).

		@returns Wartoœæ zwrócona przez funkcjê java.

		@throws JvmException - w czasie wykonania funkcji statycznej wyst¹pi³
		wyj¹tek java.
		*/
		template<typename JniType>
		JniType callStaticMethod(jclass classId, jmethodID methodID, const ArgsArray& args = ArgsArray()) {
			return callStaticMethod<JniType>(classId, methodID, args.size() ? &args[0] : NULL);
		}

		/** Wariant funkcji umo¿liwiaj¹cy zwracanie wartoœci typu innego ni¿
		typ, na rzecz którego jest wywo³ywana, bez potrzeby rzutowania
		w kodzie wywo³uj¹cym. Najczêœciej stosowana w przypadku funkcji, które zwracaj¹
		referencje typu pochodnego od <code>jobcject</code>. Wówczas przydaje siê ona
		w np. takich sytuacjach:

		\code
		//z koniecznoœci¹ jawnego rzutowania:
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
		Wywo³uje statyczn¹ funcjê java.

		@tparam JniType - typ zwracany wywo³ywanej funkcji statycznej. Na podstawie tego
		    parametru rozstrzygane jest, która funkcja œrodowiska java jest
			wywo³ywana (CallStaticVoidMethod, CallStaticIntMethod, itp).

		@param classId - id klasy, na rzecz której ma zostaæ wywo³ana sk³adowa funkcja
		statyczna.
		@param methodID - identyfikator metody, która ma zostaæ wywo³ana.
		@param args - c-tablica zawieraj¹ca argumenty, które maj¹ zostaæ przekazane
		do funkcji. Mo¿e byæ NULL - oznacza to brak argumentów.

		@returns Wartoœæ zwrócona przez funkcjê java.

		@throws JvmException - w czasie wykonania funkcji statycznej wyst¹pi³
		wyj¹tek java.
		*/
		template<typename JniType>
		JniType callStaticMethod(jclass classId, jmethodID methodID, const jvalue* args) {

			JniType retval;
			retval = _Traits::FunPtrs<JniType>::CallStaticMethodA(env, classId, methodID, args);			

			checkException();

			return retval;
		}

		/** Tworzy now¹ tablicê java obiektów typu prostego.
		@tparam JniType - typ obiektów, których tablica ma zostaæ utworzona. 
			Nie mo¿e byæ <code>jobject</code> ani pochodny, w tym celu nale¿y u¿yæ
			newArray(jsize, jclass).

		@param size - wielkoœæ tablicy.

		@return Nowa tablica java.

		@throws JvmException - nast¹pi³ wyj¹tek java.
		*/
		template <typename JniType> typename _Traits::Types<JniType>::ArrayType newArray(jsize size) {

			typename _Traits::Types<JniType>::ArrayType arr = _Traits::FunPtrs<JniType>::NewArray(env, size);
			checkException();
			return arr;;
		}

		/** Tworzy tablicê java obiektów typu <code>jobject</code> lub pochodnego.
		@param size - wielkoœæ tablicy.
		@param clazz - id klasy java, której zawiera tablica.

		@return Nowa tablica obiektów typu okreœlonego przez <code>clazz</code>,
			zainicjalizowana wartoœciami <code>null</code>.

		@throws JvmException - nast¹pi³ wyj¹tek java.
		*/
		_Traits::Types<jobject>::ArrayType newArray(jsize size, jclass clazz) {

			_Traits::Types<jobject>::ArrayType arr = _Traits::FunPtrs<jobject>::NewArray(env, size, clazz, NULL);
			checkException();
			return arr;
		}

		/** Konwertuje tablicê natywn¹ na tablicê java.
		Przydatna przy przesy³aniu argumentów z funkcji natywnej do funkcji java.
		
		Obecnie nie istnieje wersja tej funkcji dla tablic przechowuj¹cych obiekty
		typu <code>jobject</code> lub pochodnego - zaleca siê tworzenie interfejsu
		w taki sposób, aby do funkcji java przesy³aæ argumenty jak najprostszych typów
		(podstawowych lub ich tablice).

		@tparam - typ JNI obiektów zawartych w tablicy. Mo¿e byæ to tylko typ podstawowy.

		@param nativeArray - tablica natywna do przekonwertowania
		@return Tablica java zawieraj¹ca elementy typu JniType. Ma tak¹ sam¹ wielkoœæ,
		jak tablica wejœciowa, i zawiera dok³adnie te same referencje.

		@throws JvmException - nast¹pi³ wyj¹tek java.
		*/
		template<typename JniType>
		typename _Traits::Types<JniType>::ArrayType toJavaArray(const std::vector<JniType>& nativeArray) {
			
			typename _Traits::Types<JniType>::ArrayType resultArray;
			resultArray = newArray<JniType>(nativeArray.size());

			_Traits::FunPtrs<JniType>::SetArrayRegion(env, resultArray, 0, nativeArray.size(), &nativeArray[0]);
			checkException();

			return resultArray;
		}

		/** Konwertuje tablicê java zawieraj¹c¹ obiekty typu prostego na tablicê
		natywn¹.
		Funkcja pomocna w aplikacji natywnej przy odbieraniu tablic java
		zwróconych przez funckje java.
		
		@tparam JniType - typ obiektów przechowywanych w tablicy.

		@param javaArray - tablica java do przekonwertowania

		@return Tablica natywna zawieraj¹ca te same referencje, które zawarte by³y
			w wejœciowej tablicy java.

		@throws JvmException - nast¹pi³ wyj¹tek java.
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


		/** Funkcja pomocnicza, która pozwala na konwersjê tablicy java
		przechowuj¹cej obiekty typu pochodnego od <code>jobject</code>
		na tablicê natywn¹ zawieraj¹c¹ tak¿e obiekty typu pochodnego od
		<code>jobject</code>. Dziêki temu podczas pracy ze zwrócon¹ tablic¹
		nie jest konieczne rzutowanie jej elementów:

		\code
		//z u¿yciem funkcji toNativeArray<jobject>(jobjectArray javaArray):
		jobjectArray strings = getStrings(); //funkcja zwracaj¹ca tablicê java stringów
		std::vector<jobject> nativeArray = toNativeArray<jobject>(strings);
		jstring someString = static_cast<jstring>(nativeArray[index]); //konieczne rzutowanie

		//z u¿yciem tej funkcji pomocniczej:
		jobjectArray strings = getStrings(); //funkcja zwracaj¹ca tablicê java stringów
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

	/** Specjalizacja dla wywo³ywania funkcji nie zwracaj¹cych  wartoœci.
	@see callStaticMethod(jclass, jmethodID, const jvalue*)
	@returns Nie zwraca wartoœci.
	*/
	template<>
	void JvmEnvironment::callStaticMethod<void>(jclass classId, jmethodID methodID, const jvalue* args);


	/** Specjalizacja dla tablic java przechowuj¹cych obiekty typu
	<code>jobject</code>.
	
	@see toNativeArray(typename _Traits::Types<JniType>::ArrayType)
	*/
	template <>
	std::vector<jobject> JvmEnvironment::toNativeArray<jobject>(jobjectArray javaArray);


	/** Pomocnicza klasa wspomagaj¹ca RAII podczas pracy z lokalnymi referencjami
	zwracanymi przez funkcje JVM. Obiekty klasy staj¹ siê w³aœcicielami referencji,
	i zwalniaj¹ j¹ w chwili wywo³ania destruktora.

	@tparam JniType - typ referencji, która jest owijana. Domyœlnie <code>jobject</code>.
	*/
	template <typename JniType = jobject>
	class LocalRef {

	private:
		LocalRef(const LocalRef& );
		LocalRef& operator= (const LocalRef& );

		JNIEnv* env_;	
		JniType ref_;

	public:

		/** Udostêpnia uchwyt do przechowywanej referencji.
		@returns Owijana referencja.
		*/
		JniType get() const {
			return ref_;
		}

		/** Konstruktor.
		@param ref - referencja, która ma zostaæ zwolniona. Mo¿e byæ NULL.
		@param env - œrodowisko java, z którego pochodzi referencja. */
		LocalRef(JniType ref, JNIEnv* env) : env_(env), ref_(ref) {
		}

		/** Destruktor. Zwalnia owijan¹ referencjê. */
		~LocalRef() {
			env_->DeleteLocalRef(ref_);
		}
	};

	/**
	Klasa reprezentuj¹ca ogólny wyj¹tek, który wystapi³ podczas wykonywania kodu
	java. W razie potrzeby mo¿na wyprowadziæ od tej klasy bardziej szczegó³ow¹
	hierarchiê wyj¹tków.
	*/
	class JvmException : public std::runtime_error {

	protected:
	
		/** Nazwa klasy wyj¹tku Java, który spowodowa³ wyst¹pienie tego wyj¹tku.*/
		std::string className_;

		/** Stacktrace wyj¹tku. */
		std::string stackTrace_;

		
		/** Konstruktor tworz¹cy obiekt wyj¹tku java z podanym komunikatem, oraz
		wype³nionym stacktrace. Powinien byæ u¿ywany tylko w chwili obs³ugi wyj¹tku
		java, który wyst¹pi³ w maszynie wirtualnej. */
		JvmException(const std::string& msg, const std::string& className, const std::string& stacktrace) : 
			std::runtime_error(msg),
			className_(className),
			stackTrace_(stacktrace) {}

	public:
		/** Zwraca stacktrace java wyj¹tku. W przypadku, gdy w czasie pobierania
		informacji o wyj¹tku wyst¹pi³ kolejny b³¹d, stacktrace mo¿e byæ pusty.
		@return Stacktrace wyj¹tku. */
		std::string getStackTrace() const {return stackTrace_; }

		/** Zwraca nazwê klasy java wyj¹tku. W przypadku, gdy w czasie pobierania
		informacji o wyj¹tku wyst¹pi³ kolejny b³¹d, nazwa mo¿e byæ pusta.
		@return Nazwa klasy Java wyj¹tku. */
		virtual std::string getClassName() const {return className_; }

		/** Konstruktor. Tworzy wyj¹tek JVM z opisem dostarczonym przez u¿ytkownika. */
		JvmException(const std::string& msg) : std::runtime_error(msg) {};
		

		~JvmException() throw() {};
		
		friend class javacall::JvmEnvironment;
		
		
	};

} //namespace javacall


#endif //UNIZETO_JAVACALL_JVMENVIRONMENT_

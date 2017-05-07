/*
 ============================================================================
 Name        : dcaca.c
 Author      : ernesto
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 XXX: http://www.spoj.com/problems/DQUERY/
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#ifdef CACA_COMUN_LOG
#include <execinfo.h>
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#else
#include <time.h>
#include <stdarg.h>
#endif

#define CACA_COMUN_TAM_MAX_LINEA (8*30000+30000)
#define CACA_LOG_MAX_TAM_CADENA 2000

#define CACA_COMUN_BUF_STATICO (char[1000] ) { '\0' }

#define BITCH_VECTOR_NUM_BITS (sizeof(bitch_vector) * 8)

#define CACA_COMUN_ASSERT_DUROTE 0
#define CACA_COMUN_ASSERT_SUAVECITO 1
#define CACA_COMUN_ASSERT_NIMADRES 2

typedef unsigned int natural;
typedef natural tipo_dato;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_SUAVECITO
/*
 #define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_DUROTE
 */

#define assert_timeout_dummy(condition) 0;
//static inline void caca_log_debug_dummy(const char *format, ...) { }

#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_DUROTE
#define assert_timeout(condition) assert(condition);
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_SUAVECITO
#define assert_timeout(condition) if(!(condition)){sleep(10);abort();}
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_NIMADRES
#define assert_timeout(condition) 0
#endif


#ifdef CACA_COMUN_LOG
#define caca_log_debug(formato, args...) \
		do \
		{ \
			size_t profundidad = 0; \
			void *array[CACA_LOG_MAX_TAM_CADENA]; \
 			profundidad = backtrace(array, CACA_LOG_MAX_TAM_CADENA); \
			caca_log_debug_func(formato,__FILE__, __func__, __LINE__,profundidad,##args); \
		} \
		while(0);
#else
#define caca_log_debug(formato, args...) 0;
//#define caca_log_debug caca_log_debug_dummy
#endif

#define caca_comun_max(x,y) ((x) < (y) ? (y) : (x))
#define caca_comun_min(x,y) ((x) < (y) ? (x) : (y))

void caca_log_debug_func(const char *format, ...);

#define caca_comun_mapa_bitch_checa(bits, posicion, resultado) \
        __asm__ (\
                        "xor %%rdx,%%rdx\n"\
                        "movq %[bitch_posi],%%rax\n" \
                        "movq $64,%%r8\n"\
                        "divq %%r8\n"\
                        "mov $1,%[resul]\n"\
                        "mov %%rdx,%%rcx\n"\
                        "shlq %%cl,%[resul]\n"\
                        "and (%[vectors],%%rax,8),%[resul]\n"\
                        : [resul] "=b" (resultado)\
                        : [bitch_posi] "r" (posicion), [vectors] "r" (bits)\
            :"rax","rdx","rcx","r8")

static inline void caca_comun_mapa_bitch_asigna(bitch_vector *bits,
		unsigned long posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (bitch_vector) ((bitch_vector) 1 << idx_registro);

}

static inline void caca_comun_mapa_bitch_limpia(bitch_vector *bits,
		unsigned long posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] &= (bitch_vector) ~((bitch_vector) 1 << idx_registro);

}

void caca_comun_current_utc_time(struct timespec *ts) {

#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
#ifdef CACA_COMUN_LOG
	clock_gettime(CLOCK_REALTIME, ts);
#endif
#endif

}

void caca_comun_timestamp(char *stime) {
	time_t ltime;
	struct tm result;
	long ms;
	struct timespec spec;
	char parte_milisecundos[50];

	ltime = time(NULL );

#ifndef ONLINE_JUDGE
	localtime_r(&ltime, &result);
	asctime_r(&result, stime);
#endif

	*(stime + strlen(stime) - 1) = ' ';

	caca_comun_current_utc_time(&spec);
#ifndef ONLINE_JUDGE
	ms = round(spec.tv_nsec / 1.0e3);
#endif
	sprintf(parte_milisecundos, "%ld", ms);
	strcat(stime, parte_milisecundos);
}
void caca_log_debug_func(const char *format, ...) {

	va_list arg;
	va_list arg2;
	const char *PEDAZO_TIMESTAMP_HEADER = "tiempo: %s; ";
	const char *HEADER =
			"archivo: %s; funcion: %s; linea %d; nivel: %zd caca 8====D ";
	char formato[CACA_LOG_MAX_TAM_CADENA + sizeof(HEADER)
			+ sizeof(PEDAZO_TIMESTAMP_HEADER)] = { '\0' };
	char pedazo_timestamp[sizeof(PEDAZO_TIMESTAMP_HEADER) + 100] = { '\0' };
	char cadena_timestamp[100] = { '\0' };

	caca_comun_timestamp(cadena_timestamp);
	sprintf(pedazo_timestamp, PEDAZO_TIMESTAMP_HEADER, cadena_timestamp);

	strcpy(formato, pedazo_timestamp);
	strcat(formato, HEADER);
	strcat(formato, format);
	strcat(formato, "\n");

	va_start(arg, format);
	va_copy(arg2, arg);
	vprintf(formato, arg2);
	va_end(arg2);
	va_end(arg);
// XXX: http://stackoverflow.com/questions/1716296/why-does-printf-not-flush-after-the-call-unless-a-newline-is-in-the-format-strin
	setbuf(stdout, NULL );
}

#ifdef CACA_COMUN_LOG
static char *caca_comun_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo,
		char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos,
				"%u", *(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static char *caca_comun_arreglo_a_cadena_natural(natural *arreglo,
		natural tam_arreglo, char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%2u",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}
#endif
void caca_comun_strreplace(char s[], char chr, char repl_chr) {
	int i = 0;
	while (s[i] != '\0') {
		if (s[i] == chr) {
			s[i] = repl_chr;
		}
		i++;
	}
}

static inline int caca_comun_lee_matrix_long_stdin(tipo_dato *matrix,
		int *num_filas, int *num_columnas, int num_max_filas,
		int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	tipo_dato numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char *linea = NULL;

	linea = calloc(CACA_COMUN_TAM_MAX_LINEA, sizeof(char));

	while (indice_filas < num_max_filas
			&& fgets(linea, CACA_COMUN_TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		caca_comun_strreplace(linea, '\n', '\0');
		if (!strlen(linea)) {
			caca_log_debug("weird, linea vacia");
			continue;
		}
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
//			caca_log_debug("el numero raw %s", linea);
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
//			caca_log_debug("en col %d, fil %d, el valor %lu", indice_columnas,
//					indice_filas, numero);
			indice_columnas++;
//			caca_log_debug("las columnas son %d", indice_columnas);
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;
		caca_log_debug("las filas son %d, con clos %d", indice_filas,
				indice_columnas);
	}

	*num_filas = indice_filas;
	free(linea);
	return 0;
}

#define DCACA_MAX_NUMERO ((natural)1E7)
#define DCACA_MAX_CONSULTAS 200000
#define DCACA_MAX_NUMEROS 30000

typedef struct mo_mada {
	natural idx_query;
	natural intervalo_idx_ini;
	natural intervalo_idx_fin;
	natural resulcaca;
	natural orden;
} mo_mada;

natural tam_bloque = 0;
int ocurrencias[DCACA_MAX_NUMERO ] = { 0 };
mo_mada consultas[DCACA_MAX_CONSULTAS] = { 0 };
natural numeros[DCACA_MAX_NUMEROS] = { 0 };
natural conteo_unicos = 0;

int dcaca_ord_idx_bloque(const void *ap, const void *bp) {
	int result = 0;
	mo_mada *a = (mo_mada *) ap;
	mo_mada *b = (mo_mada *) bp;
	natural idx_bloq_a = 0;
	natural idx_bloq_b = 0;

	idx_bloq_a = a->intervalo_idx_ini / tam_bloque;
	idx_bloq_b = b->intervalo_idx_ini / tam_bloque;

	if (idx_bloq_a == idx_bloq_b) {
		result = a->idx_query - b->idx_query;
	} else {
		result = idx_bloq_a - idx_bloq_b;
	}

	return result;
}

int dcaca_ord_interv_idx_der(const void *ap, const void *bp) {
	int result = 0;
	mo_mada *a = (mo_mada *) ap;
	mo_mada *b = (mo_mada *) bp;
	natural inter_idx_der_a = 0;
	natural inter_idx_der_b = 0;
	natural idx_bloq_a = 0;
	natural idx_bloq_b = 0;

	idx_bloq_a = a->intervalo_idx_ini / tam_bloque;
	idx_bloq_b = b->intervalo_idx_ini / tam_bloque;

	inter_idx_der_a = a->intervalo_idx_fin;
	inter_idx_der_b = b->intervalo_idx_fin;

	if (idx_bloq_a == idx_bloq_b) {
		natural idx_izq_a = a->intervalo_idx_ini;
		natural idx_izq_b = b->intervalo_idx_ini;
		if (inter_idx_der_a == inter_idx_der_b) {
			if (idx_izq_a == idx_izq_b) {
				result = a->orden - b->orden;
			} else {
				result = idx_izq_a - idx_izq_b;
			}
		} else {
			result = inter_idx_der_a - inter_idx_der_b;
		}
	} else {
		result = a->orden - b->orden;
	}

	return result;
}

int dcaca_ord_idx_query(const void *ap, const void *bp) {
	int result = 0;
	mo_mada *a = (mo_mada *) ap;
	mo_mada *b = (mo_mada *) bp;
	result = a->idx_query - b->idx_query;

	return result;
}

static inline void dcaca_anadir_mierda(int *ocurrencias, tipo_dato num) {
	caca_log_debug("aumentando ocurrencias de %u", num);
	ocurrencias[num]++;
	caca_log_debug("quedaron %u", ocurrencias[num]);
	if (ocurrencias[num] == 1) {
		conteo_unicos++;
		caca_log_debug("el conteo de uniqs aumento a %u", conteo_unicos);
	}
}

static inline void dcaca_quitar_mierda(int *ocurrencias, tipo_dato num) {
	caca_log_debug("disminuiendo ocurrencias de %u", num);
	assert_timeout(ocurrencias[num] > 0);
	ocurrencias[num]--;
	caca_log_debug("quedaron %d", ocurrencias[num]);
	if (!ocurrencias[num]) {
		conteo_unicos--;
		caca_log_debug("el conteo de uniqs disminuio a %u", conteo_unicos);
	}
}

static inline mo_mada *dcaca_core(mo_mada *consultas, natural *numeros,
		natural num_consultas, natural num_numeros) {
	natural idx_izq_act = 0;
	natural idx_der_act = 0;
	tam_bloque = ceil(sqrt(num_numeros));
	caca_log_debug("total de nums %u, tam bloq %u", num_numeros, tam_bloque);

	caca_log_debug("ordendando cons x bloq");
	qsort(consultas, num_consultas, sizeof(mo_mada), dcaca_ord_idx_bloque);
	for (int i = 0; i < num_consultas; i++) {
		(consultas + i)->orden = i;
	}
	caca_log_debug("ordendando cons x limite der");

	qsort(consultas, num_consultas, sizeof(mo_mada), dcaca_ord_interv_idx_der);

	idx_izq_act = idx_der_act = (consultas)->intervalo_idx_ini;
	dcaca_anadir_mierda(ocurrencias, numeros[idx_izq_act]);

	for (int i = 0; i < num_consultas; i++) {
		natural consul_idx_izq = (consultas + i)->intervalo_idx_ini;
		natural consul_idx_der = (consultas + i)->intervalo_idx_fin;

		assert_timeout(
				ceil(abs((int )idx_izq_act - (int )consul_idx_izq))
						<= tam_bloque * 2);

		caca_log_debug("vamos a bailar %u-%u", consul_idx_izq, consul_idx_der);

		caca_log_debug("disminu izq act %u a izq consul %u", idx_izq_act,
				consul_idx_izq);
		while (idx_izq_act > consul_idx_izq) {
			idx_izq_act--;
			dcaca_anadir_mierda(ocurrencias, numeros[idx_izq_act]);
		}

		caca_log_debug("aumen der act %u a der consul %u", idx_der_act,
				consul_idx_der);
		while (idx_der_act < consul_idx_der) {
			idx_der_act++;
			dcaca_anadir_mierda(ocurrencias, numeros[idx_der_act]);
		}

		caca_log_debug("aumen izq act %u a izq consul %u", idx_izq_act,
				consul_idx_izq);
		while (idx_izq_act < consul_idx_izq) {
			dcaca_quitar_mierda(ocurrencias, numeros[idx_izq_act]);
			idx_izq_act++;
		}

		caca_log_debug("disminu der act %u a der consul %u", idx_der_act,
				consul_idx_der);
		while (idx_der_act > consul_idx_der) {
			dcaca_quitar_mierda(ocurrencias, numeros[idx_der_act]);
			idx_der_act--;
		}

		caca_log_debug("el conteo uniq de la consul %u es %u", i,
				conteo_unicos);
		(consultas + i)->resulcaca = conteo_unicos;
	}

	qsort(consultas, num_consultas, sizeof(mo_mada), dcaca_ord_idx_query);
	return consultas;
}

void dcaca_main() {
	natural num_numeros = 0;
	natural num_consultas = 0;

	scanf("%u", &num_numeros);
	caca_log_debug("el num de nums %u", num_numeros);

	caca_comun_lee_matrix_long_stdin(numeros, &(int ) { 0 }, NULL, 1,
			num_numeros);

	scanf("%u\n", &num_consultas);

	caca_log_debug("el num de consuls %u", num_consultas);

	for (int i = 0; i < num_consultas; i++) {
		mo_mada *consul_act = consultas + i;

		scanf("%u %u\n", &consul_act->intervalo_idx_ini,
				&consul_act->intervalo_idx_fin);

		consul_act->intervalo_idx_ini -= 1;
		consul_act->intervalo_idx_fin -= 1;

		consul_act->idx_query = i;
		consul_act->orden = i;
	}

	dcaca_core(consultas, numeros, num_consultas, num_numeros);

	for (int i = 0; i < num_consultas; i++) {
		caca_log_debug("consulta %u resul %u", i, (consultas + i)->resulcaca);
		printf("%u\n", (consultas + i)->resulcaca);
	}
}

int main(void) {
	dcaca_main();
	return EXIT_SUCCESS;
}

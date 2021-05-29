#include <stdlib.h>
#include <stdio.h>

int tst ();

typedef struct S {

  int *a;

} s_t;

s_t
create_s () {
  s_t s;
  s.a = malloc ( sizeof ( int ) * 5 );
  for ( int i = 0; i < 5; ++i ) {
    s.a [ i ] = i << 1;
  }
  return s;
}

void
fill_s ( s_t s ) {
  for ( int i = 0; i < 5; ++i ) {
    s.a [ i ] = i;
  }
}

void
kill_s ( s_t s ) {
  free ( s.a );
}

void
fill1_s_from_const ( s_t *s ) {
  int array [ 5 ] = { 11, 21, 31, 41, 51 };
  (*s).a = array;
}

s_t
fill2_s_from_const () {
  int array [ 5 ] = { 12, 22, 32, 42, 52 };
  s_t s;
  s.a = array;
  return s;
}

void
copy_s_from_const ( s_t s ) {
  int array [ 5 ] = { 111, 222, 333, 444, 555 };
  for ( int i = 0; i < 5; ++i ) {
    s.a [ i ] = array [ i ];
  }
}

void
abc ( const char (*p) [ 64 ], int len ) {

  for ( int i = 0; i < len; ++i ) {

    printf ( "%s\n", p [ i ] );

  }

}









void
A ( int **a ) {

  (*a) = malloc ( 3 * sizeof (int) );
  for (int i = 0; i < 3; ++i) (*a) [i] = i;

}
















double *f ( float *a ) {

  printf("%f\n", *a);

  return (double*) a;

}


int
main () {

  float aa = 12.56f;

  double *ff = f( &aa );

  printf("%f\n", *ff);








  return 0;




  int *a;

  A (&a);

  for (int i = 0; i < 3; ++i) printf("%d\n", a[i]);




  free (a);

  return 0;















  const char p [ 2 ] [ 64 ] = {
    "hello",
    "world"
  };
  abc ( p, 2 );











  #define TESTDEF 123
  tst();


  return 0;

  const char *text = "hello, world!";
  printf ( "%s\n", text );

  for ( int i = 0; i < 0xff; ++i ) {
    if ( text [i] == '\0' ) break;
    printf ( "%c %d ", text [ i ], text [ i ] );
  }
  printf ( "\n" );


  return 0;

  for ( int i = 0; i < 256; ++i ) {
    if (i % 16 == 0 && i > 0) printf("\n");
    if (i == 0) printf(" ");
    // else if (i == 1) printf(" ");
    else if (i == 7) printf(" ");
    else if (i == 8) printf(" ");
    else if (i == 9) printf(" ");
    else if (i == 10) printf(" ");
    // else if (i == 11) printf(" ");
    else if (i == 13) printf(" ");
    else if (i == 32) printf(" ");
    else if (i == 27) printf(" ");
    else if (i == 28) printf(" ");
    else printf("%c", i);
  }


  printf( "%d\n\n", 0 ^ 0 );

  /*
  s_t s = create_s ();
  printf ( "\ncreate_s\n" );
  for ( int i = 0; i < 5; ++i ) {
    printf ( "%d\n", s.a [ i ] );
  }

  fill_s ( s );
  printf ( "\nfill_s\n" );
  for ( int i = 0; i < 5; ++i ) {
    printf ( "%d\n", s.a [ i ] );
  }

  copy_s_from_const ( s );
  printf ( "\ncopy_s_from_const\n" );
  for ( int i = 0; i < 5; ++i ) {
    printf ( "%d\n", s.a [ i ] );
  }

  kill_s ( s );

  */

  s_t s;
  // not working at all
  fill1_s_from_const ( &s );
  printf ( "\nfill1_s_from_const\n" );
  for ( int i = 0; i < 5; ++i ) {
    printf ( "%d\n", s.a [ i ] );
  }

  // works partly
  s = fill2_s_from_const ();
  printf ( "\nfill2_s_from_const\n" );
  for ( int i = 0; i < 5; ++i ) {
    printf ( "%d\n", s.a [ i ] );
  }

  // same as fill1_s_from_const or fill2_s_from_const
  // but works fine
  int b [ 5 ] = { 11, 22, 33, 44, 55 };
  s.a = b;
  printf ( "\ninline\n" );
  for ( int i = 0; i < 5; ++i ) {
    printf ( "%d\n", s.a [ i ] );
  }
}

int
tst () {
  #define TEST2 666
  printf("\n\n%d\n\n", TESTDEF);
}

#!/bin/bash

asn1c rfc3125.asn1 rfc3280Explicit88.asn1 rfc3280Implicit88.asn1
#kompilacja modulu polityki podpisu (rfc 3125 , aneks A-1 )
#zgodne z plikami *.spol
# moduly z plikow rfc3280Explicit88.asn1 oraz rfc3280Implicit88.asn1 potrzebne sa do importu niektorych definicji
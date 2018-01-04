#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/utils.h"
#include "../inc/types.h"
#include "../inc/chars.h"
#include "../inc/c1.h"

bool test_char_on_str(byte c, uint8 pos, uint8 key_length, byte str[],
                      uint32 str_length) {
    // On parcourt le message
    for (uint32 i = 0; i < str_length; ++i) {
        // Si le caractère i est codé par le carac c en position pos
        if (i % key_length == pos) {
            // S'il est invalide, on sort
            if (!char_valid_message(c ^ str[i])) return false;
        }
    }
    return true;
}

void print_chars(byte chars[], uint8 nb_chars) {
    printf("[");
    for (uint8 i = 0; i < nb_chars; ++i) {
        printf("%c", chars[i]);
    }
    printf("]");
}

void extract_keys_loop(uint8 i, uint16 *i_key, byte **keys, byte **key_chars,
                       byte key_chars_length[], byte temp_key[], uint8 key_length) {
    if (i == key_length) {
        temp_key[key_length] = '\0';
        strcpy((char *) keys[*i_key], (char *) temp_key);
        ++(*i_key);
        return;
    }
    for (uint8 j = 0; j < key_chars_length[i]; ++j) {
        temp_key[i] = key_chars[i][j];
        extract_keys_loop(i + 1, i_key, keys, key_chars, key_chars_length,
                          temp_key, key_length);
    }
}

byte **extract_keys(byte **key_chars, uint32 *nb_keys, uint8 key_chars_length[],
                    uint8 key_length) {
    *nb_keys = 1;
    for (uint8 i = 0; i < key_length; ++i) *nb_keys *= key_chars_length[i];

    byte **keys = init_2d_array(*nb_keys, key_length + 1);
    byte *temp_key = init_array(key_length + 1);

    uint16 i_key = 0;
    extract_keys_loop(0, &i_key, keys, key_chars, key_chars_length,
                      temp_key, key_length);

    free_array(&temp_key);

    return keys;
}

byte **c1(byte str[], uint32 str_length, uint8 key_length,
          uint8 key_chars_length[]) {
    uint8 array_capacity = 10;
    byte **key_chars = init_2d_array(key_length, array_capacity);

    uint8 j = 0;
    // Pour chaque caractère potentiel de la clé
    for (uint8 i = 0; i < key_length; ++i) {
        j = 0;
        // On parcourt tous les caractères possibles d'une clé
        for (byte c = 44; c < 126; ++c) {
            if (char_valid_key(c)) {
                if (test_char_on_str(c, i, key_length, str, str_length)) {
                    // Le caractère c a passé tous les tests : on l'ajoute
                    // aux caractères possibles de la clé
                    key_chars[i][j] = c;
                    ++j;

                    // Si le tableau est trop petit, on augmente sa taille
                    if (j == array_capacity) {
                        expand_array(key_chars + i, MAX_NB_VALID_KEY_CHARS);
                    }
                }
            }
        }
        // si j == 0 ça veut dire que pour le ième caractère de la clé,
        // aucun caractère n'est possible : inutile de continuer
        if (j == 0) {
            // On met le premier élément à 0 pour pouvoir savoir plus
            // tard que ce tableau n'est pas complet et qu'il ne
            // faut pas l'afficher
            key_chars[i][0] = 0;
            return key_chars;
        }
        key_chars_length[i] = j;
    }
    return key_chars;
}

#!/bin/bash

COMMON_SRCS="srcs/Server.cpp srcs/Client.cpp srcs/Parser.cpp srcs/Channel.cpp srcs/Bot.cpp srcs/commands/*.cpp tests/globals.cpp"

FAILED=0

echo "=== LANCEMENT DE TOUS LES TESTS UNITAIRES ==="
echo ""

for test_file in tests/test_*.cpp; do
    test_name=$(basename "$test_file" .cpp)
    bin_name="run_$test_name"

    echo "--- Compilation et exécution de : $test_name ---"

    c++ -Wall -Wextra -Werror -std=c++98 "$test_file" $COMMON_SRCS -o "$bin_name"
    
    if [ $? -ne 0 ]; then
        echo "[ERREUR] La compilation a échoué pour $test_name"
        FAILED=$((FAILED + 1))
        echo ""
        continue
    fi

    ./"$bin_name"
    
    if [ $? -ne 0 ]; then
        echo "[ERREUR] Un test a échoué dans $test_name"
        FAILED=$((FAILED + 1))
    else
        echo "[SUCCÈS] $test_name validé !"
    fi  # Correction ici : 'fi' au lieu de 'endif'
    
    rm -f "$bin_name"
    echo ""
done

echo "=== BILAN DES TESTS ==="
if [ $FAILED -eq 0 ]; then
    echo "Tous les fichiers de test ont réussi avec succès !"
    exit 0
else
    echo "$FAILED fichier(s) de test ont rencontré des erreurs."
    exit 1
fi
#!/usr/bin/env bash

gen() {
  cat >"./fiat${1}.txt" <<EOF
num_generations: 30
num_population_size: 50
percent_crossover: 50
percent_mutation: 50
percent_elite: 15
tournament_size: 2
visualization: false
function_num: ${1}
EOF
}

if test -n "${1}"; then
  # if we have a parameter
  gen "${1}"
else
  # otherwise default to 0.. 17
  for num in {0..17}; do
    gen "${num}"
  done
fi

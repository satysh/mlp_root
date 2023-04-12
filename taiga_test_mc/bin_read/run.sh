#!/bin/bash
INPUTBINFILE=../input/bpe607_30_da0.1_md5/taiga607_st2b # gamma
#INPUTBINFILE=../input/bpe623_30_da5.0_md5/taiga623_st2b # protons
OUTDIRNAME=../output

if [ -d ${OUTDIRNAME} ]; then
  rm -fv ${OUTDIRNAME}/data.root
else
  mkdir ${OUTDIRNAME}
fi


root -l -b -q "tree_form.C(\"${INPUTBINFILE}\", \"${OUTDIRNAME}\")"
wait
root -l ${OUTDIRNAME}/data.root

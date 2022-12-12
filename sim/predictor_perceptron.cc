#include "predictor.h"
#include <limits>

/////////////// STORAGE BUDGET JUSTIFICATION ////////////////

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// Constructeur du prédicteur
PREDICTOR::PREDICTOR(char *prog, int argc, char *argv[])
{
   // La trace est tjs présente, et les arguments sont ceux que l'on désire
   if (argc != 2) {
      fprintf(stderr, "usage: %s <trace> pcbits countbits\n", prog);
      exit(-1);
   }

   uint32_t pcbits    = strtoul(argv[0], NULL, 0);
   uint32_t countbits = strtoul(argv[1], NULL, 0);

   nentries = (1 << pcbits);        // nombre d'entrées dans la table
   pcmask   = (nentries - 1);       // masque pour n'accéder qu'aux bits significatifs de PC
   countmax = (1 << countbits) - 1; // valeur max atteinte par le compteur à saturation
   h        = 59;
   h_mask   = (1 << h) - 1;
   ghist    = NOT_TAKEN;

   table_perceptron = new int32_t*[nentries]();
   for (uint32_t i = 0; i < nentries; ++i) {
      table_perceptron[i] = new int32_t[h + 1](); // +1 pour le biais
   }

}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool PREDICTOR::GetPrediction(UINT64 PC)
{
   uint32_t index = PC & pcmask;
   perceptron_vector = table_perceptron[index][0]; // biais

   for(uint32_t i = 0; i < h; i++) {
      if(ghist & (1 << i)) { perceptron_vector += table_perceptron[index][i + 1]; }
      else                 { perceptron_vector -= table_perceptron[index][i + 1]; }
   }
   
   return perceptron_vector >= 0 ? TAKEN : NOT_TAKEN;

}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void PREDICTOR::UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget)
{
   uint32_t index = PC & pcmask;
   int32_t perceptron_threshold = countmax;
   int32_t y_out = 0;
   int32_t t = (resolveDir == TAKEN) ? 1 : -1;

   if(perceptron_vector > perceptron_threshold) { y_out = 1; }
   else if(perceptron_vector < (-1) * perceptron_threshold) { y_out = -1; }

   if(y_out != t) {
       table_perceptron[index][0] += t; // biais
       for(uint32_t i = 0; i < h; i++) {
           table_perceptron[index][i + 1] += t * ((ghist & (1 << i)) >> i);
       }
   }

   ghist = ((ghist << 1) % h_mask) + (resolveDir == TAKEN ? 1 : 0);

}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void PREDICTOR::TrackOtherInst(UINT64 PC, OpType opType, bool branchDir, UINT64 branchTarget)
{
   // This function is called for instructions which are not
   // conditional branches, just in case someone decides to design
   // a predictor that uses information from such instructions.
   // We expect most contestants to leave this function untouched.
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


/***********************************************************/

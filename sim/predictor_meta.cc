#include "predictor.h"

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
   h        = min((uint32_t)10, (uint32_t)log2(nentries));
   h_mask   = (1 << h) - 1;
   
   // Métaprédicteur
   table_meta    = new uint32_t[nentries]();   // BHT métaprédicteur

   // Prédicteur global
   table_gshare   = new uint32_t[nentries]();   // BHT prédicteur global
   ghist    = NOT_TAKEN;
   
   // Prédicteur local
   table_local    = new uint32_t[nentries]();   // BHT prédicteur local
   lpht           = new uint32_t[nentries]();
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool PREDICTOR::GetPrediction(UINT64 PC)
{
   // Résultat du prédicteur gshare
   uint32_t v_gshare = table_gshare[(PC & pcmask) ^ ghist];
   res_gshare = v_gshare > countmax / 2 ? TAKEN : NOT_TAKEN;

   // Résultat du prédicteur local
   uint32_t v_local = table_local[lpht[PC & pcmask]];
   return v_local > countmax / 2 ? TAKEN : NOT_TAKEN;

   // Retour du résultat du prédicteur choisi
   uint32_t v = table_meta[PC & pcmask];
   return v > countmax / 2 ? res_gshare : res_local;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void PREDICTOR::UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget)
{
   // Mise à jour de l'historique global du prédicteur gshare
   uint32_t index_gshare = (PC & pcmask) ^ ghist;
   uint32_t v_gshare = table_gshare[index_gshare];
   table_gshare[index_gshare] = resolveDir == TAKEN ? SatIncrement(v_gshare, countmax) : SatDecrement(v_gshare);
   ghist = ((ghist << 1) % h_mask) + (resolveDir == TAKEN ? 1 : 0);

   // Mise à jour de l'historique du prédicteur local
   uint32_t lhist = lpht[PC & pcmask];
   uint32_t v_local = table_local[lhist];
   table_local[lhist] = resolveDir == TAKEN ? SatIncrement(v_local, countmax) : SatDecrement(v_local);
   lpht[PC & pcmask] = ((lhist << 1) % h_mask) + ((resolveDir == TAKEN) ? 1 : 0);

   // Mise à jour du métaprédicteur
   uint32_t v = table_meta[PC & pcmask];
   bool increment_cond  = (resolveDir == res_gshare && res_gshare != res_local)
                        | (resolveDir == res_local && res_gshare != res_local);
   table_meta[PC & pcmask] = increment_cond ? SatIncrement(v, countmax) : SatDecrement(v);
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

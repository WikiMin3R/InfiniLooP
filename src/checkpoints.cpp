// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

 namespace Checkpoints
 {
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0000,    uint256("0x000001ed1ed7503c6ef472314d8138b7440ebd0c5c8b539481705032bbed295a") ) // genesis
        ( 7500,    uint256("0x097cb27ca7f98df07fcb5b069f3252f636dc2842a18fdb90dc1c3fca2ac7f462") ) // last pow block
        ( 27310,   uint256("0xb7195b00e1ae01a42bf43b6469a1a9297265e12daebbbeaa26b15cd5001a1ef4") ) // January 31, 2021 checkpoint
        ( 318599,  uint256("0xef2c3952ea2a63f40e5a2498f6732386c768f2727021e381f43a26f36efa940e") ) // September 1, 2021 checkpoint
        ( 400001,  uint256("0x54fac03ec53c1eb81df641a6db15725bfa7113edbc9a2ba905e6d265db2069c7") ) // October 30, 2021 checkpoint
        ( 500001,  uint256("0x65c1ca4b5c8a68af0cb2fa972d6e3540b03c5ca1c0766da5c1e79f682a9c8e5d") ) // January 10, 2022 checkpoint
        ( 600001,  uint256("0x279f4cf9848fd39a06d054bdd37e1a0d5f814e4b3d2eb9244d872f3a2a4c64db") ) // March 24, 2022 checkpoint
        ( 675001,  uint256("0xf824d1fad4373c2ecd1a0f97c0ed950df8acfb79cd4d7606169e2caa5e0a9e79") ) // May 18, 20221 checkpoint
;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}

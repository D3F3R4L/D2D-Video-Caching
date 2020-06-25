/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2016 Technische Universitaet Berlin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ESBA_ALGORITHM_H
#define ESBA_ALGORITHM_H

#include "tcp-stream-adaptation-algorithm.h"
#include "ns3/li-ion-energy-source.h"
#include "ns3/energy-source.h"
#include "ns3/li-ion-energy-source-helper.h"

namespace ns3 {

/**
 * \ingroup tcpStream
 * \brief Implementation of the ESBA adaptation algorithm
 */
class ESBAAlgorithm : public AdaptationAlgorithm
{
public:
  ESBAAlgorithm (  const videoData &videoData,
                      const playbackData & playbackData,
                      const bufferData & bufferData,
                      const throughputData & throughput);

  algorithmReply GetNextRep (const int64_t segmentCounter, int64_t clientId);

  double throughputEstimation ();

  double StallProbability(int64_t bs);

private:
  const int64_t m_buffer;
  const int64_t m_delta;
  const double m_window;
  const int64_t m_highestRepIndex;
  const int64_t m_escapeRepIndex;
  const int64_t m_lowestRepIndex;
  const double m_theta;
  const int64_t m_minimumSegments;
};

} // namespace ns3
#endif /* FESTIVE_ALGORITHM_H */
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

#include "ESBA.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ESBAAlgorithm");

NS_OBJECT_ENSURE_REGISTERED (ESBAAlgorithm);

ESBAAlgorithm::ESBAAlgorithm (  const videoData &videoData,
                                      const playbackData & playbackData,
                                      const bufferData & bufferData,
                                      const throughputData & throughput) :
  AdaptationAlgorithm (videoData, playbackData, bufferData, throughput),
  m_buffer (60000000),
  m_delta (5),
  m_window (5),
  m_highestRepIndex (videoData.averageBitrate.size () - 1),
  m_escapeRepIndex (0),
  m_lowestRepIndex (1),
  m_theta (0.3),
  m_minimumSegments (5)
{}

double
ESBAAlgorithm::throughputEstimation () // Usei a função do festive 
{
   // compute throughput estimation
  std::vector<double> thrptEstimationTmp;
  for (unsigned sd = m_playbackData.playbackIndex.size (); sd-- > 0; )
    {
      if (m_throughput.bytesReceived.at (sd) == 0)
        {
          continue;
        }
      else
        {
          thrptEstimationTmp.push_back ((8.0 * m_throughput.bytesReceived.at (sd))
                                        / ((double)((m_throughput.transmissionEnd.at (sd) - m_throughput.transmissionRequested.at (sd)) / 1000000.0)));
        }
      if (thrptEstimationTmp.size () == m_window)
        {
          break;
        }
    }
  // calculate harmonic mean of values in thrptEstimationTmp
  double harmonicMeanDenominator = 0;
  for (uint i = 0; i < thrptEstimationTmp.size (); i++)
    {
      harmonicMeanDenominator += 1 / (thrptEstimationTmp.at (i));
    }
  double thrptEstimation = thrptEstimationTmp.size () / harmonicMeanDenominator;
  return thrptEstimation*0.8;
}

double
ESBAAlgorithm::StallProbability(int64_t bs) // P(t-delta)
{
  double Nbs=0;
  double Nb=0;
  uint k = m_bufferData.bufferLevelNew.size()-1;
  for (uint i = 0; i < m_delta; i++)
  {
    if (m_bufferData.bufferLevelNew.at(k-i)<bs)
    {
      Nbs++;
      Nb++;
    }
    else
    {
      Nb++;
    }
  }
  NS_LOG_UNCOND(Nbs/Nb);
  return Nbs/Nb;
}

algorithmReply
ESBAAlgorithm::GetNextRep (const int64_t segmentCounter, int64_t clientId)
{ //variaveis inerentes do algoritmo de adaptação ------- 
  int64_t timeNow = Simulator::Now ().GetMicroSeconds ();
  algorithmReply answer;
  answer.decisionTime = timeNow;
  answer.nextDownloadDelay = 0;
  answer.delayDecisionCase = 0;
  //-----------------------------------------------------

  int64_t Rcur = 0; // Current bitrate level 
  int64_t Bt = 0; // buffer no momento
  if (segmentCounter>0)
  {
    Rcur = m_playbackData.playbackIndex.back (); // Current bitrate level 
    Bt = m_bufferData.bufferLevelNew.back () - (timeNow - m_throughput.transmissionEnd.back ()); // buffer no momento
  }
  int64_t bcont=m_buffer/2; // 50% do buffer
  int64_t bs=m_buffer/4; // 25% do buffer
  int64_t bov=3*m_buffer/4; // 75% do buffer
  Ptr<LiIonEnergySource> LIES = client->GetObject<LiIonEnergySource> ();
  double joules = LIES->GetRemainingEnergy ();
  if (joules>0) // Para o teste de energia 
  {
    if (segmentCounter < m_minimumSegments) // Para os primeiros segmentos
      {
        answer.nextRepIndex = 0;
        answer.decisionCase = 0;
        return answer;
      }
    if (bcont<Bt and Bt<bov)
      {
        if (Rcur<m_highestRepIndex && throughputEstimation()>m_videoData.averageBitrate.at (Rcur) and joules>0) // true é do teste de energia
        {
          answer.nextRepIndex = Rcur+1;
          answer.decisionCase = 1;
          return answer;
        }
        if (throughputEstimation()>=m_videoData.averageBitrate.at (Rcur) and Rcur==m_highestRepIndex and joules>0)
        {
          answer.nextRepIndex = Rcur;
          answer.decisionCase = 2;
          return answer;
        }
        else
        {
          answer.nextRepIndex = Rcur;
          answer.decisionCase = 3;
          return answer;
        }
      }
    else
    {
      if (bs<Bt and Bt<bcont)
      {
        if (throughputEstimation()<m_videoData.averageBitrate.at (Rcur) and Rcur>m_lowestRepIndex)
        {
          answer.nextRepIndex = Rcur-1;
          answer.decisionCase = 4;
          return answer;
        }
        else
        {
          if (Rcur<m_highestRepIndex && throughputEstimation()>m_videoData.averageBitrate.at (Rcur) and joules>0) // true é do teste de energia
          {
            answer.nextRepIndex = Rcur+1;
            answer.decisionCase = 5;
            return answer;
          }
          if (throughputEstimation()>=m_videoData.averageBitrate.at (Rcur) and Rcur==m_highestRepIndex and joules>0)
          {
            answer.nextRepIndex = Rcur;
            answer.decisionCase = 6;
            return answer;
          }
          else
          {
            answer.nextRepIndex = Rcur;
            answer.decisionCase = 7;
            return answer;
          }
        }
      }
      else
      {
        if (Bt<=bs and Rcur>=m_escapeRepIndex)
        {
          if (StallProbability(bs)<m_theta)
          {
            answer.nextRepIndex = m_lowestRepIndex;
            answer.decisionCase = 8;
            return answer;
          }
          else
          {
            answer.nextRepIndex = m_escapeRepIndex;
            answer.decisionCase = 9;
            return answer;
          }
        }
        if (Bt>=bov)
        {
          answer.nextRepIndex = Rcur;
          answer.decisionCase = 10;
          return answer;
        }
      }
    }
  }
  else
  {
    if(Rcur>m_lowestRepIndex)
    {
      answer.nextRepIndex = Rcur-1;
      answer.decisionCase = 11;
      return answer;
    }
  }
}
} // namespace ns3
#pragma once
#include "../math/vector.h"
#include "../state_machine/state_machine.h"
#include "entities/player.h"
#include "goal.h"
#include "soccer_pitch.h"
#include <vector>

class SoccerTeam {
public:
    enum TeamColor { RED, BLUE };

private:
    Goal *p_HomeGoal;
    Goal *p_AwayGoal;
    SoccerPitch *p_Pitch;
    SoccerTeam *p_Opponent;

    StateMachine<SoccerTeam> m_StateMachine;
    TeamColor m_Color;

    Player *p_GoalKeeper;
    Player *p_ControllingPlayer;
    Player *p_ReceivingPlayer;
    Player *p_PlayerClosestToBall;
    Player *p_SupportingPlayer;
    double m_DistanceSQToBallFromClosestPlayer;

    Vector m_BestSupportSpot;
    double m_TimeSinceLastUpdateToSupportSpot;

    struct Spot {
        Vector position;
        int rate;
    };

    private:
    /**
    * @brief Calculates the closest player to the ball,
    * updates p_PlayerClosestToBall.
    */
    void CalcClosestPlayerToBall();

    void CreatePlayers();

    bool GetBestPassToReceiver(const Player *const passer,
                                const Player *const reciever, Vector &passTarget,
                                const double power) const;

    void CheckSpot(Spot &spot) const;

    /**
    * @brief Calculates the best support spot to support the controlling player.
    * @return The best support spot as a Vector.
    */
    Vector GetBestSupportSpot();

public:
    std::vector<Player *> m_Players;

public:
    SoccerTeam(TeamColor color, Goal *homeGoal, Goal *oppGoal, SoccerPitch *pitch)
        : p_HomeGoal(homeGoal), p_AwayGoal(oppGoal), p_Pitch(pitch),
            p_Opponent(nullptr), m_Color(color), p_GoalKeeper(nullptr),
            p_ControllingPlayer(nullptr), p_ReceivingPlayer(nullptr),
            p_PlayerClosestToBall(nullptr),
            m_DistanceSQToBallFromClosestPlayer(0.0) {}

    ~SoccerTeam();

    void Initialize(SoccerTeam *opponent);

    void Update();

    StateMachine<SoccerTeam> FSM() { return m_StateMachine; }

    inline SoccerPitch* Pitch() const { return p_Pitch; }

    inline Goal* HomeGoal() const { return p_HomeGoal; }

    inline Goal* AwayGoal() const { return p_AwayGoal; }

    inline Player* ReceivingPlayer() const { return p_ReceivingPlayer; }

    inline Player* SupportingPlayer() const { return p_SupportingPlayer; }

    inline void SetSupportingPlayer(Player* player) { p_SupportingPlayer = player; }

    inline void setControllingPlayer(Player* player) { p_ControllingPlayer = player; }

    inline void SetReceivingPlayer(Player* player) { p_ReceivingPlayer = player; }

    bool InControl() const { return p_ControllingPlayer != nullptr; }

    void DetermineBestSupportingPlayer();

    inline Vector BestSupportingSpot() {
        return m_BestSupportSpot;
    }

    void ReturnFieldPlayersToHome();

    bool AllPlayersAtHome() const;

    bool IsPassSafeFromOpponent(const Vector &target,
                                const Player *const reciever,
                                const Player *const opponent,
                                const double power) const;

    bool IsPassSafeFromAllOpponents(const Vector &target,
                                    const Player *const reciever,
                                    const double power) const;

    bool CanShoot(const Vector &ball_pos, const double power,
                    Vector &shotTarget) const;

    void RequestPass(const Player *const requester) const;

    bool FindPass(const Player *const passer, Player *&receiver,
                    Vector &passTarget, const double power,
                    const double minPassDistance) const;
};

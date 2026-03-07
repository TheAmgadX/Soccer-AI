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

    StateMachine<SoccerTeam>* p_StateMachine;
    TeamColor m_Color;

    Player *p_GoalKeeper;
    Player *p_ControllingPlayer;
    Player *p_ReceivingPlayer;
    Player *p_PlayerClosestToBall;
    Player *p_SupportingPlayer;
    Player *p_SecondarySupportingPlayer;
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
            p_SupportingPlayer(nullptr),
            p_SecondarySupportingPlayer(nullptr),
            m_DistanceSQToBallFromClosestPlayer(0.0),
            m_BestSupportSpot(Vector()),
            m_TimeSinceLastUpdateToSupportSpot(0.0) {}

    ~SoccerTeam();

    void Initialize(SoccerTeam *opponent);

    void Update();

    TeamColor Color() const { return m_Color; }

    StateMachine<SoccerTeam>* FSM() { return p_StateMachine; }

    inline SoccerPitch* Pitch() const { return p_Pitch; }

    inline SoccerTeam* Opponent() const { return p_Opponent; }

    inline Goal* HomeGoal() const { return p_HomeGoal; }

    inline Goal* AwayGoal() const { return p_AwayGoal; }

    inline Player* ReceivingPlayer() const { return p_ReceivingPlayer; }

    inline Player* SupportingPlayer() const { return p_SupportingPlayer; }
    inline Player* SecondarySupportingPlayer() const { return p_SecondarySupportingPlayer; }

    inline Player* ControllingPlayer() const { return p_ControllingPlayer; }

    inline void SetSupportingPlayer(Player* player) {
        p_SupportingPlayer = player;
        if (player == nullptr) {
            p_SecondarySupportingPlayer = nullptr;
        }
    }
    inline void SetSecondarySupportingPlayer(Player* player) { p_SecondarySupportingPlayer = player; }
    inline void ClearSupportingPlayer(const Player* player) {
        if (p_SupportingPlayer == player) {
            p_SupportingPlayer = nullptr;
        }
        if (p_SecondarySupportingPlayer == player) {
            p_SecondarySupportingPlayer = nullptr;
        }
    }

    inline void setControllingPlayer(Player* player) { p_ControllingPlayer = player; }

    inline void SetReceivingPlayer(Player* player) { p_ReceivingPlayer = player; }

    inline void SetClosestPlayerToBall(Player* player) { p_PlayerClosestToBall = player; }

    inline Player* ClosestPlayerToBall() const { return p_PlayerClosestToBall; }

    bool InControl() const { return p_ControllingPlayer != nullptr; }

    void DetermineBestSupportingPlayer();
    void UpdateSupportSpots();

    inline Vector BestSupportingSpot() {
        return m_BestSupportSpot;
    }
    Vector SupportSpotFor(const Player* player) const;

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

    void RequestPass(Player * requester) const;

    bool FindPass(const Player *const passer, Player *&receiver,
                    Vector &passTarget, const double power,
                    const double minPassDistance) const;
};

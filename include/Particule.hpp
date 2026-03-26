#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include <string>
#include <array>

using Vec2 = std::array<double, 2>;

class Particule{
    private:
        int id;
        std::string type;
        double masse;
        Vec2 position;
        Vec2 vitesse;
        Vec2 force;

    public:
        void afficher() const;
        Particule(int id, std::string type, double masse, Vec2 pos, Vec2 vitesse);

        // Getters
        int getId() const;
        std::string getType() const;
        double getMasse() const;
        Vec2 getPosition() const;
        Vec2 getVitesse() const;
        Vec2 getForce() const; 
        
        // Setters
        void setPosition(const Vec2& pos);
        void setVitesse(const Vec2& vitesse);
        void setForce(const Vec2& f);

        void ajouterForce(Vec2& f);
};

#endif
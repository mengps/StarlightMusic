import QtQuick 2.12
import QtQuick.Particles 2.12

Item {
    id: root
    property alias system: particleSystem
    property alias source: particleImage.source
    property alias emitRate: particlesEmitter.emitRate

    ParticleSystem {
        id: particleSystem
        running: true

        Emitter {
            id: particlesEmitter
            enabled: true
            height: root.height
            width: root.width
            system: particleSystem
            size: 14
            sizeVariation: 8
            endSize: 0
            lifeSpan: 1300
            lifeSpanVariation: 800
            maximumEmitted: 400
            velocity: AngleDirection {
                angle: -90
                angleVariation: 40
                magnitude: 40
                magnitudeVariation: 40
            }
            acceleration: AngleDirection {
                angle: -90
                magnitude: 120
            }
        }

        ImageParticle {
            id: particleImage
            system: particleSystem
            alpha: 0.65
            alphaVariation: 0.25
            colorVariation: 0.6
            /*rotation: 90
            rotationVariation: 60
            rotationVelocity: 180
            rotationVelocityVariation: 90*/
        }
    }
}

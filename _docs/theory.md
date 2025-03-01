---
layout: single
title: Theory
permalink: /docs/theory/
---

Here we discuss the high-level strategy, define that strategy using dynamics and an assumed friction model, and how that model was validated.

## High-Level Strategy
Given a controllable moving surface, and a part that is to be transported, the operating principle is relatively simple and similar to that of horizontal transport[^1]. First, the part is transported upwards against gravity by _sticking_ to the moving surface. The moving surface then quickly accelerates downward in order to _slip_ relative to the part, catching the part at a lower position. This sticking-slipping cycle repeats to have net motion of the part upwards, as shown in the below graphic.

![image-center]({{ site.url }}{{ site.baseurl }}/assets/images/vibrational-strategy.gif){: .align-center}
<span class="figure-caption"><strong>Fig. 1</strong>: Alternating sticking and slipping phases achieve net part transport against gravity.</span>

[^1]: Quaid, A. E. (1999, May). A miniature mobile parts feeder: Operating principles and simulation results. In Proceedings 1999 IEEE International Conference on Robotics and Automation (Cat. No. 99CH36288C) (Vol. 3, pp. 2221-2226). IEEE.

## Dynamics
While many friction models exist, we used the well-known Coulomb friction model. The equations of motion are shown below:

\begin{equation}
\quad \text{sticking:} \quad \dot{z}_P = \dot{z}_S, \quad -\frac{\mu_s F_n}{m_P} -g \leq \ddot{z}_S \leq \frac{\mu_s F_n}{m_P} - g \label{eq:sticking}\\
\end{equation}
\begin{equation}
\quad \text{slipping:} \quad \ddot{z}_P = \frac{\mu_k F_n}{m_P} \text{sgn}(\dot{z}_S - \dot{z}_P) - g, \quad \dot{z}_P \neq \dot{z}_S \label{eq:slipping}
\end{equation}

### Vertical Vibratory Transport Is Difficult

From $\eqref{eq:sticking}$, the mimimun required normal force to prevent the part from accelerating downwards when the surface is stationary ($\small \dot{z}_S = 0$) is the following:

\begin{equation}
F_n > \frac{m_p g}{\mu_s} \label{eq:minimum_normal}
\end{equation}

From $\eqref{eq:sticking}$ and $\eqref{eq:minimum_normal}$ the minimum required peak acceleration from the surface actuator, $a_{max}$, is the following:

\begin{equation}\label{eq:a_max_lower_bound}
    a_{max} > \mu_s F_n/m_P + g > 2g
\end{equation}

The above equations detail the challenges which upward vertical vibratory transport presents compared to horizontal transport. In most practical cases, we have $\mu_s < 1$, so \eqref{eq:minimum_normal} means that the part $P$ must be squeezed with a force $F_n$ exceeding its own weight. Equation $\eqref{eq:sticking}$ shows that gravity reduces the maximum upward part acceleration during sticking, and that overcoming this limitation requires squeezing the part harder still. However, from \eqref{eq:a_max_lower_bound}, squeezing with higher normal forces requires more powerful actuators to reach higher surface accelerations, which already need to exceed $2g$ (compared to $a_{max} > \mu_s g$ for the horizontal case). Finally, \eqref{eq:slipping} shows that during slipping, the part accelerates faster down than up.

## Experimental Validation

To validate our dynamics model described by $\eqref{eq:sticking}$ and $\eqref{eq:slipping}$ we recorded the interaction of a moving surface and a transported part. The recording was then processed by the free software [Tracker](https://opensourcephysics.github.io/tracker-website/) in order to extract surface and part motion. The surface motion was then used as input to a Simulink model to predict the resulting part motion, which was compared with the experimentally observed part motion. A sample trial (Trial #5) is shown below (there were 10 total trials).

![image-center]({{ site.url }}{{ site.baseurl }}/assets/images/part-position-comparison.png){: .align-center}

<span class="figure-caption"><strong>Fig. 2</strong>: Simulated (red) and experimental (black) part positions, along with the periodic surface motion (blue).</span>
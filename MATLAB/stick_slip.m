% Calculate dry friction force from platform on object

function out = stick_slip(u)

F_other = u(1); % [N] forces other than dry friction
xp_dot = u(2);  % [m/s] platform velocity
xp_ddot = u(3); % [m/s/s] platform acceleration
xo_dot = u(4);  % [m/s] object velocity
m_o = u(5); % [kg] object mass
F_n = u(6); % [N] normal force from platform on object
mu_s = u(7); % [-] static friction coefficient
mu_k = u(8); % [-] kinetic friction coefficient
v_min = u(9); % [m/s] threshold to check if slipping or sticking

F_s = mu_s * F_n; % [N] maximum Coulomb static friction force
F_k = mu_k * F_n; % [N] Coulomb kinetic friction force
F_hyp = m_o * xp_ddot - F_other; % [N] hypothetical friction force if there is stiction
v_rel = xo_dot - xp_dot; % [N] object velocity relative to platform

sticking = 0; % [bool] 0 if slipping, 1 if sticking

if abs(v_rel) >= v_min % if slipping
    F_friction = - sign(v_rel) * F_k;
else % sticking
    if abs(F_hyp) > F_s % if got unstuck
        F_friction = -sign(v_rel) * F_k; 
    else % stuck
        F_friction = F_hyp;
        sticking = 1;
    end
end

out = [F_friction, sticking];

end

